#include <ConstraintChecks/CheckBBN.h>

//using namespace std;

//  CheckBBN::CheckBBN(/* args */){}
//  CheckBBN::~CheckBBN(){}
/*
vector<vector<double>> CheckBBN::parseBbnData(string infile){
    ifstream file;
    file.open(infile);
    string line;
    vector<vector<double>> fileData;
    while(getline(file, line)){
        auto split = FileIO::Split(line, " =\t\0");
        vector<double> lineData;
        for (int i = 0; i < split.size(); ++i){
            double parsedData = FileParser::ParseDouble(split[i]);
            lineData.push_back( parsedData );
        }
        fileData.push_back( lineData );
    }
    return fileData;
}

double CheckBBN::hadronicBranchingRatio(ScaleFactorParticleProperties particle, ScaleFactorParticleProperties radiation, ModelBase model){
    double BRhad = 0.;
    if (particle.Particle.Key == "saxion" || particle.Particle.Key == "axino" || particle.Particle.Key == "gravitino"){
        BRhad = 1.;
    } else if (particle.Particle.Key == "axion"){
        BRhad = 0.;
    } else if (particle.Particle.Key == "neutralino1"){
        double hadWidth = 0.;
        for (auto& w : particle.BranchFraction.PartialWidths){
            if (w.Children.size() == 2){
                if (
                    (w.Children[0].Key == "upq" && w.Children[1].Key == "upq")
                    ||
                    (w.Children[0].Key == "downq" && w.Children[1].Key == "downq")
                    ||
                    (w.Children[0].Key == "strangeq" && w.Children[1].Key == "strangeq")
                    ||
                    (w.Children[0].Key == "charmq" && w.Children[1].Key == "charmq")
                    ||
                    (w.Children[0].Key == "bottomq" && w.Children[1].Key == "bottomq")
                    // TODO: find why top quark is excluded (should be kinematically allowed for Z1 >~ 350 GeV)
                ){
                    hadWidth += w.Width;
                }
            } 

        }
        BRhad = hadWidth / particle.BranchFraction.TotalWidth.Width;
    }
    return BRhad;
}

double CheckBBN::radiationPreRelicDensity(ScaleFactorParticleProperties particle, ScaleFactorParticleProperties radiation, ModelBase model){
    double preRelicDensity = particle.ParticleDensities.PreRelicDensity;

    if (particle.Particle.Key == "saxion"){
//        preRelicDensity = particle.ParticleDensities.PreRelicDensity * StableDecays::Br(particle, radiation, model);
    }
    return preRelicDensity;
}

CheckBBN::DataElement CheckBBN::findNearestData(string infile, double BRhad, double logLifetime){
    int posBr = posNearestBr(BRhad);
    vector<vector<double>> fileData = parseBbnData(infile);
    DataElement data;
    int posLifetime = 0;
    for (posLifetime = 0; posLifetime < fileData.size(); ++posLifetime){
        auto lineData = fileData[posLifetime];
        if (lineData[0] >= logLifetime){
            auto lastElement = fileData[posLifetime];
            data.UpperLifetime = lastElement[0];
            // Upper X,Y values for interpolation
            data.UpperOmega = lastElement[posBr];
            break;
        }
    }

    if (posLifetime > 0){
        auto dataVminus2 = fileData[posLifetime - 1];
        data.LowerLifetime = dataVminus2[0];
        // Lower X,Y values for interpolation
        data.LowerOmega = dataVminus2[posBr];
    } else{
        // If lifetime outside list bounds, use first 2 points for extrapolation 
        auto firstPoint = fileData[0];
        auto secondPoint = fileData[1];
        data.LowerLifetime = firstPoint[0];
        // Lower X,Y values for interpolation
        data.LowerOmega = firstPoint[posBr];
        data.UpperLifetime = secondPoint[0];
        // Lower X,Y values for interpolation
        data.UpperOmega = secondPoint[posBr];
    }
    
    // If tau > taumax (10^12), use constant limit on omega:
    if (data.LowerLifetime == 0. && data.UpperLifetime == 0. && data.LowerOmega == 0. && data.UpperOmega == 0.){
        auto lastPoint = fileData.end()[-1];
        data.LowerLifetime = lastPoint[0];
        data.UpperLifetime = logLifetime;
        data.LowerOmega = lastPoint[posBr];
        data.UpperOmega = lastPoint[posBr];
    }

    return data;
}

// This is a direct translation from OG code
int CheckBBN::posNearestBr(double BRhad){
    // Find closest BRhad:
    double logBr = log10(BRhad);
    double DBR = abs(logBr);
    int posBr = 0;
    for(int IRI = 0; IRI < 10; ++IRI){
        double RI = 0. - IRI*0.5;
        double DBR0 = abs(logBr - RI);
        if (DBR0 <= DBR){
            // posBr = position of closest BR value
            posBr = IRI + 1; 
            DBR = DBR0;
        }
    }
    return posBr;
}

double CheckBBN::interpolatedOmega(CheckBBN::DataElement data, double logLifetime){
    double omega = ((data.UpperOmega-data.LowerOmega)*logLifetime + (data.LowerOmega*data.UpperLifetime - data.UpperOmega*data.LowerLifetime)) / (data.UpperLifetime-data.LowerLifetime);
    return omega;
}

void CheckBBN::bbnOK(ScaleFactorParticleProperties& particle, ScaleFactorParticleProperties radiation, ModelBase model){
    double BRhad = hadronicBranchingRatio(particle, radiation, model);
    double preRelicDensity = radiationPreRelicDensity(particle, radiation, model);

    if (log10(BRhad) < -5. || log10(BRhad) > 0.){
        throw logic_error("Input outside range");
    }

    vector<string> bbnFiles;
    bbnFiles.push_back("cards/config/BBNdata1.dat");
    bbnFiles.push_back("cards/config/BBNdata2.dat");

    double logOmegaA = 0.;
    double logOmegaB = 0.;
    double lifetime = 0.;
    if ( particle.BranchFraction.TotalWidth.Width > 0. ){
        lifetime = model.Constants.hBar / particle.BranchFraction.TotalWidth.Width;
    }
    double logLifetime = log10(lifetime);

    // Find point with closest Tau in data files
    for (auto& infile : bbnFiles){
        auto data = findNearestData(infile, BRhad, logLifetime);

        if(infile == "cards/config/BBNdata1.dat"){
            // Interpolated value (in log scale) for mZ1 = 1 TeV
            logOmegaA = interpolatedOmega(data, logLifetime);
        } else{
            // Interpolated value (in log scale) for mZ1 = 100 GeV
            logOmegaB = interpolatedOmega(data, logLifetime);
        }
    }

    // Now interpolate in the masses
    double omegaConstraint = (
        (logOmegaA-logOmegaB) * log10(particle.Particle.Mass) 
        + 
        ( logOmegaB * log10(1000.) - logOmegaA * log10(100.) ) 
    ) / ( log10(1000.) - log10(100.) );

    // Check if BBN bound is satisfied
    double omegaCalculated = log10( preRelicDensity );

    if (omegaCalculated < omegaConstraint){
        particle.BbnConstraint = 1;
    } else{
        particle.BbnConstraint = -1;
    }
}

void CheckBBN::CheckConstraint(ModelBase model, ScaleFactor& finalPoint){
    auto radiation = finalPoint.Find(finalPoint.Particle, "photon");
    for (auto& p : finalPoint.Particle){
        if(p.TempDecay > 0. && p.Particle.Key != "photon"){
            // Only apply bounds to heavy gravitinos (mGT > mZ1)
            if (p.Particle.Key == "gravitino" && p.Particle.Mass < ModelBaseOps::FindConst(model.Particles, "neutralino1").Mass){
                continue;
            }
            bbnOK(p, radiation, model);
        }
    }

}
*/
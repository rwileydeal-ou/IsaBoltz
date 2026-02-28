#include <cmd/CheckBBN/Receiver.h>

CheckBBNReceiver::CheckBBNReceiver(
    Connection& connection, 
    DbManager& db,
    Models::Particle& particle, 
    const Models::ParticleEvolution& particleEvolution, 
    const Models::ScaleFactorPoint& scaleFactor, 
    const Models::TotalWidth& totalWidth
) :
    connection_(connection),
    db_(db)
{
    particle_ = particle;
    particleEvolution_ = particleEvolution;
    scaleFactor_ = scaleFactor;
    totalWidth_ = totalWidth;
    partialWidths_ = pullPartialWidths();
    hadrons_ = pullHadrons();
    checkBBN_.InputId = connection.InputId;
    checkBBN_.ParticleId = particle.Id;
    checkBBN_.ScaleFactorId = scaleFactor.Id;
    checkBBN_.ParticleEvolutionId = particleEvolution.Id;
}
CheckBBNReceiver::~CheckBBNReceiver(){
}

Models::TotalWidth CheckBBNReceiver::pullTotalWidth(){
    Models::TotalWidth br;

    auto statement = Statements::TotalWidth(
        br, 
        Statements::StatementType::Read
    );
    auto filter = Filters::TotalWidth(
        particle_.InputId, 
        particle_.Id, 
        scaleFactor_.Id
    );
    statement.AddFilter(filter);
    auto cb = Callbacks::TotalWidth();
    db_.Execute(
        statement, 
        cb.Callback, 
        cb.CallbackReturn
    );

    if (cb.CallbackReturn.TotalWidths.size() != 1){
        throw_with_trace( std::logic_error("Could not find unique TotalWidth") );
    }

    br = cb.CallbackReturn.TotalWidths.front();
    return br;
}

std::vector< Models::PartialWidth > CheckBBNReceiver::pullPartialWidths(){
    std::vector< Models::PartialWidth > children;
    Models::PartialWidth child;

    auto statement = Statements::PartialWidth(
        child, 
        Statements::StatementType::Read
    );
    auto filter = Filters::PartialWidth(
        connection_.InputId, 
        particle_.Id, 
        scaleFactor_.Id
    );
    statement.AddFilter( filter );
    auto cb = Callbacks::PartialWidth();
    db_.Execute( 
        statement, 
        cb.Callback, 
        cb.CallbackReturn 
    );

    for (size_t i=0; i < cb.CallbackReturn.PartialWidths.size(); ++i){
        children.push_back( cb.CallbackReturn.PartialWidths[i] );
    }

    return children;
}

std::vector< Models::Particle > CheckBBNReceiver::pullHadrons(){
    std::vector< Models::Particle > hadrons;
    Models::Particle hadron;

    auto statement = Statements::Particle(
        hadron, 
        Statements::StatementType::Read
    );
    auto filter = Filters::Particle(
        connection_.InputId, 
        Filters::WhereUUID::InputId
    );
    statement.AddFilter( filter );
    auto cb = Callbacks::Particle();
    db_.Execute( 
        statement, 
        cb.Callback, 
        cb.CallbackReturn 
    );

    for (size_t i=0; i < cb.CallbackReturn.Particles.size(); ++i){
        auto p = cb.CallbackReturn.Particles[i];
        // this is a hack. TODO: we really need to not hardcode things. Especially when they're in the config file and SM content.
        // TODO: find why top quark is excluded (should be kinematically allowed for Z1 >~ 350 GeV)
        if (p.Key == "upq" || p.Key == "downq" || p.Key == "strangeq" || p.Key == "charmq" || p.Key == "bottomq"){
            hadrons.push_back( p );
        }
    }

    return hadrons;
}

void CheckBBNReceiver::Calculate(){
    if(
        totalWidth_.Width == 0. // doesn't decay - we're ok
        || scaleFactor_.Temperature > 1.e-2 // bail if above 10MeV, should be fine
        || particle_.Key == "photon"
    ){
        checkBBN_.ConstraintSatisfied = true;
        return;
    }

    // TODO: should we check m_input < m_LSP here and short circuit?  
    // BR should return 0 but probably no need to make the calls

    // Find point with closest Tau in data files
    // TODO: yeah... not this...
    std::string bbnFile1 = "cards/config/BBNdata1.dat";
    std::string bbnFile2 = "cards/config/BBNdata2.dat";

    // we already validated width > 0
    double logLifetime = log10( connection_.Model.Constants.hBar / totalWidth_.Width );
    if (logLifetime < -2.0) {
        checkBBN_.ConstraintSatisfied = true;
        return;
    }
    checkBBN_.BrHadronic = hadronicBranchingRatio();

    // Interpolated value (in log scale) for mZ1 = 1 TeV
    double logOmegaA = interpolatedOmega(bbnFile1, logLifetime);
    // Interpolated value (in log scale) for mZ1 = 100 GeV
    double logOmegaB = interpolatedOmega(bbnFile2, logLifetime);

    if ( particle_.Mass >= 1000. ){
        checkBBN_.DensityConstraint = logOmegaA;
    } else if ( particle_.Mass <= 100. ){
        checkBBN_.DensityConstraint = logOmegaB;
    } else{
        // interpolate in the masses
        checkBBN_.DensityConstraint = (
            (logOmegaA - logOmegaB) * log10(particle_.Mass) 
            + 
            ( 3. * logOmegaB - 2. * logOmegaA ) 
        );
    }

    // Check if BBN bound is satisfied
    checkBBN_.DensityCalculated = log10( relicDensityBeforeDecay() );

    if (checkBBN_.DensityCalculated < checkBBN_.DensityConstraint){
        checkBBN_.ConstraintSatisfied = true;
    } else{
        checkBBN_.ConstraintSatisfied = false;
    }
}

Models::CheckBBN CheckBBNReceiver::getCheckBBN(){
    return checkBBN_;
}

double CheckBBNReceiver::hadronicBranchingRatio(){
    double BRhad = 0.;
    if (particle_.Key == "saxion" || particle_.Key == "axino" || particle_.Key == "gravitino" || particle_.Key == "modulus"){
        BRhad = 1.;
    } else if (particle_.Key == "neutralino1"){
        return 1.; // TODO: REMOVE THIS HACK. THIS WORKS FOR RPV FOR NOW
        for (auto& w : partialWidths_){
	    // requires >= 3-body decay (R-odd particle + q qbar, or l q q'bar if RPV allowed)
            if (w.ChildrenIds.size() != 3){
                continue;
            }
            bool isHadronicDecay = false;
            for (size_t i=0; i < w.ChildrenIds.size(); ++i){ 
                auto childId = w.ChildrenIds[i];
                auto hadDecay = find_if(
                    hadrons_.begin(),
                    hadrons_.end(),
                    [&childId](const Models::Particle& p) {return p.Id == childId;}
                );
            
                if (hadDecay != hadrons_.end()){
                    isHadronicDecay = true;
                }
            }
    	    if (isHadronicDecay){
	        	BRhad += w.Width / totalWidth_.Width;
            }
        }
    }

    // let's just validate stuff was good for the current set of files.
    // TODO: we really need a better programmatic approach to handle updating constraints...
    if (log10(BRhad) < -5. || log10(BRhad) > 0.){
        throw std::logic_error("Input outside range");
    }

    return BRhad;
}

std::vector<std::vector<double>> CheckBBNReceiver::parseBbnData(
    std::string infile
){
    std::ifstream file;
    file.open(infile);
    std::string line;
    std::vector<std::vector<double>> fileData;
    while(getline(file, line)){
        auto split = FileIO::Split(line, " =\t\0");
        std::vector<double> lineData;
        for (size_t i = 0; i < split.size(); ++i){
            double parsedData = FileParser::ParseDouble(split[i]);
            lineData.push_back( parsedData );
        }
        fileData.push_back( lineData );
    }
    return fileData;
}

double CheckBBNReceiver::relicDensityBeforeDecay(){
    // note - also need BR -> radiation factor here
    double omega = particleEvolution_.EnergyDensity * 2.741e8 / (scaleFactor_.Entropy / pow(scaleFactor_.ScaleFactor, 3.));
    return omega;
}

// This is a direct translation from OG code
// I don't fully understand it. Let's hope it works...
unsigned int CheckBBNReceiver::indexBr(){
    double logBr = log10(checkBBN_.BrHadronic);
    // since BR->had <=1, log10 will be <=0, so take abs - still well defined
    double abslogBr = abs(logBr);
    unsigned int index = 0;
    for(int i = 0; i < 10; ++i){
        // find closest `bin' based on abs(log(br) + n/2)
        double binnedBr = abs(logBr + i*0.5);
        if (binnedBr <= abslogBr){
            index = i + 1;
            abslogBr = binnedBr;
        }
    }
    return index;
}

double CheckBBNReceiver::interpolatedOmega(
    std::string infile, 
    double logLifetime
){
    unsigned int posBr = indexBr();
    std::vector<std::vector<double>> fileData = parseBbnData(infile);
    unsigned int fileIndex = 0;

    double upperLifetime = 0.;
    double lowerLifetime = 0.;
    double upperOmega = 0.;
    double lowerOmega = 0.;

    for (fileIndex = 0; fileIndex < fileData.size(); ++fileIndex){
        auto rowDataUpper = fileData[fileIndex];
        if (rowDataUpper[0] >= logLifetime){
            // Upper X,Y values for interpolation
            upperLifetime = rowDataUpper[0];
            upperOmega = rowDataUpper[posBr];

            if (fileIndex > 0){
                // Lower X,Y values for interpolation
                auto rowDataLower = fileData[fileIndex - 1];
                lowerLifetime = rowDataLower[0];
                lowerOmega = rowDataLower[posBr];
            } else{
                // If lifetime outside list bounds, use first 2 points for extrapolation 
                auto firstPoint = fileData[0];
                lowerLifetime = firstPoint[0];
                lowerOmega = firstPoint[posBr];
                auto secondPoint = fileData[1];
                upperLifetime = secondPoint[0];
                upperOmega = secondPoint[posBr];
            }
        
            // we got the first entry that exceeds logLifetime for interpolation
            break;
        }
    }

    // If tau > taumax (10^12), use constant limit on omega:
    if (
        lowerLifetime == 0. 
        && upperLifetime == 0. 
        && lowerOmega == 0. 
        && upperOmega == 0.
    ){
        auto lastPoint = fileData.back();
        lowerLifetime = lastPoint[0];
        upperLifetime = logLifetime;
        lowerOmega = lastPoint[posBr];
        upperOmega = lastPoint[posBr];
    }

    double omega = (
        (upperOmega - lowerOmega)*logLifetime 
        + 
        (lowerOmega * upperLifetime - upperOmega * lowerLifetime)
    ) / (upperLifetime - lowerLifetime);
    return omega;
}

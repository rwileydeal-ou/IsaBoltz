#include <ModelIndependent/CrossSections/CrossSectionFromFile.h>

CrossSectionFromFile::CrossSectionFromFile( std::string fileName )
{
    fileName_ = fileName;
}

CrossSectionFromFile::~CrossSectionFromFile()
{
}

SigmaV CrossSectionFromFile::Calculate(const Models::Particle& particle, const ModelBase& model, double T, std::shared_ptr< DataRelay > fortInterface){
    if (T == 0.){
        return SigmaV( particle, T, 0. );
    }
    std::ifstream inFile( fileName_.c_str() );
    std::string line;
    if (inFile.is_open()){

        int sigvCol = -1;
        int tempCol = -1;
        double previousTemp = 0.;
        double currentTemp = 0.;
        double previousSigV = 0.;
        double currentSigV = 0.;

        while ( getline (inFile,line) ){
            boost::char_separator<char> sep(",");
            boost::tokenizer< boost::char_separator<char> > cols(line, sep);
            int i = 0;
            for(auto& col : cols) {
                if ( i == tempCol ){
                    previousTemp = currentTemp;
                    currentTemp = stold( col );
                    if ( currentTemp <= T && previousTemp >= T ){
                        if (currentTemp == T){
                            inFile.close();
                            return SigmaV( particle, T, currentSigV );
                        } else if ( previousTemp == T ){
                            inFile.close();
                            return SigmaV( particle, T, previousSigV );
                        } else if ( previousTemp > T && previousTemp > 0.){
                            double lin = ( currentSigV - previousSigV ) / ( currentTemp - previousTemp );
                            double cs = lin * ( T - currentTemp ) + currentSigV;
                            if ( currentSigV == previousSigV ){
                                cs = currentSigV;
                            }
                            inFile.close();
                            return SigmaV( particle, T, cs );
                        }
                    }
                }
                if ( i == sigvCol ){
                    previousSigV = currentSigV;
                    currentSigV = stold( col );
                }

                if ( col == "SigmaV" ){
                    sigvCol = i;
                } else if ( col == "Temperature" ){
                    tempCol = i;
                }

                i++;
            }
        }
        inFile.close();
    }
    throw_with_trace( std::logic_error("Could not read correct SigmaV from file at T: " + boost::lexical_cast<std::string>(T)+" GeV") );
    throw std::logic_error("If you can read this, something went very wrong...");
}

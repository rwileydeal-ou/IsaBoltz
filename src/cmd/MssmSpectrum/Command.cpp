#include <cmd/MssmSpectrum/Command.h>

MssmSpectrumCommand::MssmSpectrumCommand(Connection& connection) : 
    connection_(connection) 
{
    this -> fortLib_ = std::make_shared< DataRelay >(connection_.Log);
    this -> receiver_ = std::make_shared< MssmSpectrumReceiver >( connection_, fortLib_ );
    this -> receiver_ -> Calculate();
}

MssmSpectrumCommand::~MssmSpectrumCommand(){
}

std::shared_ptr< DataRelay > MssmSpectrumCommand::getFortranInterface(){
    if (this -> fortLib_){
        return fortLib_;
    }
}

void MssmSpectrumCommand::Execute(){
}

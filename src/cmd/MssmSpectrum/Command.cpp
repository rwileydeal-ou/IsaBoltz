#include <cmd/MssmSpectrum/Command.h>

MssmSpectrumCommand::MssmSpectrumCommand(
    Connection& connection,
    DbManager& db
) : 
    connection_(connection),
    db_(db)
{
    this -> fortLib_ = std::make_shared< DataRelay >(connection_.Log);
    this -> receiver_ = std::make_shared< MssmSpectrumReceiver >( 
        connection_, 
        db_,
        fortLib_ 
    );
    this -> receiver_ -> Calculate();
}

MssmSpectrumCommand::~MssmSpectrumCommand(){
}

std::shared_ptr< DataRelay > MssmSpectrumCommand::getFortranInterface(){
    if (this -> fortLib_){
        return fortLib_;
    }
    throw std::logic_error("If you can read this, something went very wrong...");
}

void MssmSpectrumCommand::Execute(){
}

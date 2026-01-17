#include <sql/Statements/BoltzmannParticleEvolution.h>

using namespace std;
using namespace Statements;

BoltzmannParticleEvolution::BoltzmannParticleEvolution(const Models::ParticleEvolution& boltzmannParticleEvolution, StatementType statementType) :
    IStatement(statementType)
{
    boltzmannParticleEvolution_ = boltzmannParticleEvolution;
}

BoltzmannParticleEvolution::~BoltzmannParticleEvolution()
{
}

void BoltzmannParticleEvolution::Validate(){
    switch (statementType_)
    {
        case Create:
            validateCreate();
            break;
        case Read:
            break;
        case Delete:
            break;
        default:
            throw_with_trace( NotImplementedException() );
    }
}

void BoltzmannParticleEvolution::validateCreate(){
    if ( boltzmannParticleEvolution_.Id.is_nil()){
        throw_with_trace( logic_error("Invalid Id") );
    }
    if ( boltzmannParticleEvolution_.InputId.is_nil() ){
        throw_with_trace( logic_error("Invalid InputId") );
    }
    if ( boltzmannParticleEvolution_.ScaleFactorId.is_nil() ){
        throw_with_trace( logic_error("Invalid ScaleFactorId") );
    }
    if ( boltzmannParticleEvolution_.ParticleId.is_nil() ){
        throw_with_trace( logic_error("Invalid ParticleId") );
    }
    if ( boltzmannParticleEvolution_.ParticleKey.empty() ){
        throw_with_trace( logic_error("Invalid ParticleKey") );
    }
}

string BoltzmannParticleEvolution::TableName(){
    return "ParticleEvolution";
}

vector<string> BoltzmannParticleEvolution::ColumnNames(){
    vector<string> columns;
    columns.reserve(20);
    columns.push_back("ID");
    columns.push_back("InputID");
    columns.push_back("ParticleKey");
    columns.push_back("ParticleID");
    columns.push_back("Production");
    columns.push_back("Active");
    columns.push_back("Y1");
    columns.push_back("Y2");
    columns.push_back("NumberDensity");
    columns.push_back("EqNumberDensity");
    columns.push_back("EnergyDensity");
    columns.push_back("ScaleFactorID");
    columns.push_back("IsOsc");
    columns.push_back("IsDecay");
    return columns;
}

vector<string> BoltzmannParticleEvolution::ColumnData(){
    vector<string> data;
    data.reserve(20);
    data.push_back("'" + boost::lexical_cast<string>(boltzmannParticleEvolution_.Id) + "'");
    data.push_back("'" + boost::lexical_cast<string>(boltzmannParticleEvolution_.InputId) + "'");
    data.push_back("'" + boltzmannParticleEvolution_.ParticleKey + "'");
    data.push_back("'" + boost::lexical_cast<string>(boltzmannParticleEvolution_.ParticleId) + "'");
    data.push_back( boost::lexical_cast<string>(boltzmannParticleEvolution_.ProductionMechanism) );
    data.push_back( boost::lexical_cast<string>(boltzmannParticleEvolution_.IsActive) );
    data.push_back( numericGuard(boltzmannParticleEvolution_.Y1) );
    data.push_back( numericGuard(boltzmannParticleEvolution_.Y2) );
    data.push_back( numericGuard(boltzmannParticleEvolution_.NumberDensity) );
    data.push_back( numericGuard(boltzmannParticleEvolution_.EquilibriumNumberDensity) );
    data.push_back( numericGuard(boltzmannParticleEvolution_.EnergyDensity) );
    data.push_back("'" + boost::lexical_cast<string>(boltzmannParticleEvolution_.ScaleFactorId) + "'");

    // only record the IsOsc value for Coh.Osc types, otherwise just put null
    string isOsc = "NULL";
    if ( boltzmannParticleEvolution_.ProductionMechanism == ParticleProductionMechanism::COHERENT_OSCILLATION ){
        isOsc = boost::lexical_cast<string>(boltzmannParticleEvolution_.IsOscillating);
    }
    data.push_back( isOsc );
    data.push_back( boost::lexical_cast<string>(boltzmannParticleEvolution_.IsDecaying) );
    return data;
}
#include <sql/Statements/RelicDensity.h>

using namespace std;
using namespace Statements;

RelicDensity::RelicDensity(const Models::RelicDensity& relicDensity, StatementType statementType) : 
    IStatement(statementType),
    relicDensity_(relicDensity)
{
}

RelicDensity::~RelicDensity()
{
}

void RelicDensity::Validate(){
    switch (statementType_)
    {
        case Create:
            validateCreate();
            break;
        case Read:
            break;
        default:
            throw_with_trace( NotImplementedException() );
    }
}
 
void RelicDensity::validateCreate(){
    if ( relicDensity_.Id.is_nil()){
        throw_with_trace( logic_error("Invalid Id") );
    }
    if ( relicDensity_.InputId.is_nil() ){
        throw_with_trace( logic_error("Invalid InputId") );
    }
    if ( relicDensity_.ParticleId.is_nil() ){
        throw_with_trace( logic_error("Invalid ParticleId") );
    }
    if ( relicDensity_.ParticleEvolutionId.is_nil() ){
        throw_with_trace( logic_error("Invalid ParticleEvolutionId") );
    }
    if ( relicDensity_.OmegaHSqr < 0. ){
        throw_with_trace( logic_error("Invalid OmegaHSqr") );
    }
}

string RelicDensity::TableName(){
    return "RelicDensity";
}

vector<string> RelicDensity::ColumnNames(){
    vector<string> columns;
    columns.reserve(10);
    columns.push_back("ID");
    columns.push_back("InputID");
    columns.push_back("ParticleID");
    columns.push_back("ParticleEvolutionID");
    columns.push_back("OmegaHSqr");
    columns.push_back("ScaleFactorID");
    return columns;
}

vector<string> RelicDensity::ColumnData(){
    vector<string> data;
    data.reserve(10);
    data.push_back( "'" + boost::lexical_cast<std::string>(relicDensity_.Id) + "'");
    data.push_back( "'" + boost::lexical_cast<std::string>(relicDensity_.InputId) + "'");
    data.push_back( "'" + boost::lexical_cast<std::string>(relicDensity_.ParticleId) + "'");
    data.push_back( "'" + boost::lexical_cast<std::string>(relicDensity_.ParticleEvolutionId) + "'");
    data.push_back( numericGuard(relicDensity_.OmegaHSqr) );

    if (relicDensity_.ScaleFactorId.is_nil()){
        data.push_back("NULL");
    } else{
        data.push_back( "'" + boost::lexical_cast<std::string>(relicDensity_.ScaleFactorId) + "'");
    }

    return data;
}

#include <sql/Statements/TempDecay.h>

using namespace std;
using namespace Statements;

TempDecay::TempDecay(const Models::TempDecay& tempDecay, StatementType statementType) : 
    IStatement(statementType)
{
    tempDecay_ = tempDecay;
}

TempDecay::~TempDecay()
{
}

void TempDecay::Validate(){
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
 
void TempDecay::validateCreate(){
    if ( tempDecay_.Id.is_nil()){
        throw_with_trace( logic_error("Invalid Id") );
    }
    if ( tempDecay_.InputId.is_nil() ){
        throw_with_trace( logic_error("Invalid InputId") );
    }
    if ( tempDecay_.ParticleId.is_nil() ){
        throw_with_trace( logic_error("Invalid ParticleId") );
    }
    if ( tempDecay_.Temperature < 0. ){
        throw_with_trace( logic_error("Invalid Temperature") );
    }
    if ( tempDecay_.GStar < 0. ){
        throw_with_trace( logic_error("Invalid GStar") );
    }
    if ( tempDecay_.GStarEntropic < 0. ){
        throw_with_trace( logic_error("Invalid GStarEntropic") );
    }
}

string TempDecay::TableName(){
    return "TempDecay";
}

vector<string> TempDecay::ColumnNames(){
    vector<string> columns;
    columns.reserve(10);
    columns.push_back("ID");
    columns.push_back("InputID");
    columns.push_back("ParticleID");
    columns.push_back("Temperature");
    columns.push_back("GStar");
    columns.push_back("GStarEntropic");
    columns.push_back("ScaleFactorID");
    return columns;
}

vector<string> TempDecay::ColumnData(){
    vector<string> data;
    data.reserve(10);
    data.push_back( "'" + boost::lexical_cast<std::string>(tempDecay_.Id) + "'");
    data.push_back( "'" + boost::lexical_cast<std::string>(tempDecay_.InputId) + "'");
    data.push_back( "'" + boost::lexical_cast<std::string>(tempDecay_.ParticleId) + "'");
    data.push_back( numericGuard(tempDecay_.Temperature) );
    data.push_back( boost::lexical_cast<std::string>(tempDecay_.GStar) );
    data.push_back( boost::lexical_cast<std::string>(tempDecay_.GStarEntropic) );

    if (tempDecay_.ScaleFactorId.is_nil()){
        data.push_back("NULL");
    } else{
        data.push_back( "'" + boost::lexical_cast<std::string>(tempDecay_.ScaleFactorId) + "'");
    }

    return data;
}

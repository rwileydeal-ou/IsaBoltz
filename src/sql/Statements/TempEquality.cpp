#include <sql/Statements/TempEquality.h>

using namespace std;
using namespace Statements;

TempEquality::TempEquality(const Models::TempEquality& tempEquality, StatementType statementType) : 
    IStatement(statementType)
{
    tempEquality_ = tempEquality;
}

TempEquality::~TempEquality()
{
}

void TempEquality::Validate(){
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
 
void TempEquality::validateCreate(){
    if ( tempEquality_.Id.is_nil()){
        throw_with_trace( logic_error("Invalid Id") );
    }
    if ( tempEquality_.InputId.is_nil() ){
        throw_with_trace( logic_error("Invalid InputId") );
    }
    if ( tempEquality_.ParticleId.is_nil() ){
        throw_with_trace( logic_error("Invalid ParticleId") );
    }
    if ( tempEquality_.Temperature < 0. ){
        throw_with_trace( logic_error("Invalid Temperature") );
    }
    if ( tempEquality_.GStar < 0. ){
        throw_with_trace( logic_error("Invalid GStar") );
    }
    if ( tempEquality_.GStarEntropic < 0. ){
        throw_with_trace( logic_error("Invalid GStarEntropic") );
    }
}

string TempEquality::TableName(){
    return "TempEquality";
}

vector<string> TempEquality::ColumnNames(){
    vector<string> columns;
    columns.reserve(10);
    columns.push_back("ID");
    columns.push_back("InputID");
    columns.push_back("ParticleID");
    columns.push_back("Production");
    columns.push_back("Temperature");
    columns.push_back("GStar");
    columns.push_back("GStarEntropic");
    columns.push_back("ScaleFactorID");
    return columns;
}

vector<string> TempEquality::ColumnData(){
    vector<string> data;
    data.reserve(10);
    data.push_back( "'" + boost::lexical_cast<std::string>(tempEquality_.Id) + "'");
    data.push_back( "'" + boost::lexical_cast<std::string>(tempEquality_.InputId) + "'");
    data.push_back( "'" + boost::lexical_cast<std::string>(tempEquality_.ParticleId) + "'");
    data.push_back( "'" + boost::lexical_cast<string>(tempEquality_.ProductionMechanism) + "'" );
    data.push_back( numericGuard(tempEquality_.Temperature) );
    data.push_back( boost::lexical_cast<std::string>(tempEquality_.GStar) );
    data.push_back( boost::lexical_cast<std::string>(tempEquality_.GStarEntropic) );

    if (tempEquality_.ScaleFactorId.is_nil()){
        data.push_back("NULL");
    } else{
        data.push_back( "'" + boost::lexical_cast<std::string>(tempEquality_.ScaleFactorId) + "'");
    }

    return data;
}

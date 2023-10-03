#include <sql/Statements/TempOsc.h>

using namespace std;
using namespace Statements;

TempOsc::TempOsc(const Models::TempOscillation& tempOsc, StatementType statementType) : 
    IStatement(statementType)
{
    tempOsc_ = tempOsc;
}

TempOsc::~TempOsc()
{
}

void TempOsc::Validate(){
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
 
void TempOsc::validateCreate(){
    if ( tempOsc_.Id.is_nil()){
        throw_with_trace( logic_error("Invalid Id") );
    }
    if ( tempOsc_.InputId.is_nil() ){
        throw_with_trace( logic_error("Invalid InputId") );
    }
    if ( tempOsc_.ParticleId.is_nil() ){
        throw_with_trace( logic_error("Invalid ParticleId") );
    }
    if ( tempOsc_.Temperature < 0. ){
        throw_with_trace( logic_error("Invalid Temperature") );
    }
    if ( tempOsc_.GStar < 0. ){
        throw_with_trace( logic_error("Invalid GStar") );
    }
    if ( tempOsc_.GStarEntropic < 0. ){
        throw_with_trace( logic_error("Invalid GStarEntropic") );
    }
}

string TempOsc::TableName(){
    return "TempOscillate";
}

vector<string> TempOsc::ColumnNames(){
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

vector<string> TempOsc::ColumnData(){
    vector<string> data;
    data.reserve(10);
    data.push_back( "'" + boost::lexical_cast<std::string>(tempOsc_.Id) + "'");
    data.push_back( "'" + boost::lexical_cast<std::string>(tempOsc_.InputId) + "'");
    data.push_back( "'" + boost::lexical_cast<std::string>(tempOsc_.ParticleId) + "'");
    data.push_back( numericGuard(tempOsc_.Temperature) );
    data.push_back( boost::lexical_cast<std::string>(tempOsc_.GStar) );
    data.push_back( boost::lexical_cast<std::string>(tempOsc_.GStarEntropic) );

    if (tempOsc_.ScaleFactorId.is_nil()){
        data.push_back("NULL");
    } else{
        data.push_back( "'" + boost::lexical_cast<std::string>(tempOsc_.ScaleFactorId) + "'");
    }

    return data;
}

#include <sql/Statements/ScaleFactor.h>


using namespace std;
using namespace Statements;

ScaleFactor::ScaleFactor(const Models::ScaleFactorPoint& scaleFactor, StatementType statementType) :
    IStatement(statementType)
{
    scaleFactor_ = scaleFactor;
}

ScaleFactor::~ScaleFactor()
{
}

void ScaleFactor::Validate(){
    switch (statementType_)
    {
        case Create:
            validateCreate();
            break;
        case Read:
            break;
        case Delete:
            break;
        case Update:
            break;
        default:
            throw_with_trace( NotImplementedException() );
    }
}

void ScaleFactor::validateCreate(){
    if ( scaleFactor_.Id.is_nil()){
        throw_with_trace( logic_error("Invalid Id") );
    }
    if ( scaleFactor_.InputId.is_nil() ){
        throw_with_trace( logic_error("Invalid InputId") );
    }
    if ( scaleFactor_.Temperature < 0. ){
        throw_with_trace( logic_error("Invalid Temperature") );
    }
    if ( scaleFactor_.Entropy < 0. ){
        throw_with_trace( logic_error("Invalid Entropy") );
    }
    if ( scaleFactor_.Hubble < 0. ){
        throw_with_trace( logic_error("Invalid Hubble") );
    }
    if ( scaleFactor_.GStar < 0. ){
        throw_with_trace( logic_error("Invalid GStar") );
    }
    if ( scaleFactor_.GStarEntropic < 0. ){
        throw_with_trace( logic_error("Invalid GStarEntropic") );
    }
}

string ScaleFactor::TableName(){
    return "ScaleFactor";
}

vector<string> ScaleFactor::ColumnNames(){
    vector<string> columns;
    columns.reserve(10);
    columns.push_back("ID");
    columns.push_back("InputID");
    columns.push_back("Temperature");
    columns.push_back("ScaleFactor");
    columns.push_back("Entropy");
    columns.push_back("Hubble");
    columns.push_back("GStar");
    columns.push_back("GStarEntropic");
    columns.push_back("Ordinal");
    return columns;
}

vector<string> ScaleFactor::ColumnData(){
    vector<string> data;
    data.reserve(10);
    data.push_back("'" + boost::lexical_cast<string>(scaleFactor_.Id) + "'");
    data.push_back("'" + boost::lexical_cast<string>(scaleFactor_.InputId) + "'");
    data.push_back( numericGuard(scaleFactor_.Temperature) );
    data.push_back( numericGuard(scaleFactor_.ScaleFactor) );
    data.push_back( numericGuard(scaleFactor_.Entropy) );
    data.push_back( numericGuard(scaleFactor_.Hubble) );
    data.push_back( boost::lexical_cast<std::string>(scaleFactor_.GStar) );
    data.push_back( boost::lexical_cast<std::string>(scaleFactor_.GStarEntropic) );
    data.push_back( boost::lexical_cast<std::string>(scaleFactor_.Ordinal) );
    return data;
}

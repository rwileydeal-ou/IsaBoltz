#include <sql/Statements/CrossSection.h>

using namespace std;
using namespace Statements;

CrossSection::CrossSection(const SigmaV& crossSection, StatementType statementType) : 
    IStatement(statementType)
{
    crossSection_ = crossSection;
}

CrossSection::~CrossSection()
{
}

void CrossSection::Validate(){
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

void CrossSection::validateCreate(){
    if ( crossSection_.Id.is_nil()){
        throw_with_trace( logic_error("Invalid Id") );
    }
    if ( crossSection_.InputId.is_nil() ){
        throw_with_trace( logic_error("Invalid InputId") );
    }
    if ( crossSection_.ParticleId.is_nil() ){
        throw_with_trace( logic_error("Invalid ParticleId") );
    }
    if ( crossSection_.CrossSection < 0. ){
        throw_with_trace( logic_error("Invalid CrossSection") );
    }
    if ( crossSection_.Temperature < 0. ){
        throw_with_trace( logic_error("Invalid Temperature") );
    }
}

string CrossSection::TableName(){
    return "SigmaV";
}

vector<string> CrossSection::ColumnNames(){
    vector<string> columns;
    columns.reserve(10);
    columns.push_back("ID");
    columns.push_back("InputID");
    columns.push_back("ParticleID");
    columns.push_back("SigmaV");
    columns.push_back("Temperature");
    columns.push_back("ScaleFactorID");
    return columns;
}

vector<string> CrossSection::ColumnData(){
    vector<string> data;
    data.reserve(10);
    data.push_back( "'" + boost::lexical_cast<std::string>(crossSection_.Id) + "'");
    data.push_back( "'" + boost::lexical_cast<std::string>(crossSection_.InputId) + "'");
    data.push_back( "'" + boost::lexical_cast<std::string>(crossSection_.ParticleId) + "'");
    data.push_back( numericGuard(crossSection_.CrossSection) );
    data.push_back( numericGuard(crossSection_.Temperature) );
    data.push_back( "'" + boost::lexical_cast<std::string>(crossSection_.ScaleFactorId) + "'");
    return data;
}

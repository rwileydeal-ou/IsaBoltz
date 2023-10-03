#include <sql/Statements/PartialWidth.h>

using namespace std;
using namespace Statements;

PartialWidth::PartialWidth(const Models::PartialWidth& partialWidth, StatementType statementType) : 
    IStatement(statementType)
{
    partialWidth_ = partialWidth;
}

PartialWidth::~PartialWidth()
{
}

void PartialWidth::Validate(){
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

void PartialWidth::validateCreate(){
    if ( partialWidth_.Id.is_nil()){
        throw_with_trace( logic_error("Invalid Id") );
    }
    if ( partialWidth_.InputId.is_nil() ){
        throw_with_trace( logic_error("Invalid InputId") );
    }
    if ( partialWidth_.ParentId.is_nil() ){
        throw_with_trace( logic_error("Invalid ParentId") );
    }
    if ( partialWidth_.TotalWidthId.is_nil() ){
        throw_with_trace( logic_error("Invalid TotalWidthId") );
    }
    for (auto& c : partialWidth_.ChildrenIds){
        if ( c.is_nil() ){
            throw_with_trace( logic_error("Invalid ChildId") );
        }
    }
    if ( partialWidth_.Width < 0. ){
        throw_with_trace( logic_error("Invalid Width") );
    }
}

string PartialWidth::TableName(){
    return "PartialWidth";
}

vector<string> PartialWidth::ColumnNames(){
    vector<string> columns;
    columns.reserve(10);
    columns.push_back("ID");
    columns.push_back("InputID");
    columns.push_back("TotalWidthID");
    columns.push_back("ParentID");
    columns.push_back("ChildrenIDs");
    columns.push_back("PartialWidth");
    columns.push_back("ScaleFactorID");
    return columns;
}

vector<string> PartialWidth::ColumnData(){
    vector<string> data;
    data.reserve(10);
    data.push_back( "'" + boost::lexical_cast<std::string>(partialWidth_.Id) + "'");
    data.push_back( "'" + boost::lexical_cast<std::string>(partialWidth_.InputId) + "'");
    data.push_back( "'" + boost::lexical_cast<std::string>(partialWidth_.TotalWidthId) + "'");
    data.push_back( "'" + boost::lexical_cast<std::string>(partialWidth_.ParentId) + "'");

    // Need to properly concatenate all the uuids for children
    string childrenIds;
    childrenIds.append("'");
    childrenIds.append( concatenateMultipleValues( partialWidth_.ChildrenIds ) );
    childrenIds.append("'");
    data.push_back( childrenIds );

    data.push_back( boost::lexical_cast<std::string>(partialWidth_.Width) );

    if (partialWidth_.ScaleFactorId.is_nil()){
        data.push_back("NULL");
    } else{
        data.push_back( "'" + boost::lexical_cast<std::string>(partialWidth_.ScaleFactorId) + "'");
    }
    
    return data;
}
 
#include <sql/Statements/TotalWidth.h>

using namespace std;
using namespace Statements;

TotalWidth::TotalWidth(const Models::TotalWidth& totalWidth, StatementType statementType) : 
    IStatement(statementType)
{
    totalWidth_ = totalWidth;
}

TotalWidth::~TotalWidth()
{
}

void TotalWidth::Validate(){
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
 
void TotalWidth::validateCreate(){
    if ( totalWidth_.Id.is_nil()){
        throw_with_trace( logic_error("Invalid Id") );
    }
    if ( totalWidth_.InputId.is_nil() ){
        throw_with_trace( logic_error("Invalid InputId") );
    }
    if ( totalWidth_.ParentId.is_nil() ){
        throw_with_trace( logic_error("Invalid ParentId") );
    }
    if ( totalWidth_.Width < 0. ){
        throw_with_trace( logic_error("Invalid Width") );
    }
}

string TotalWidth::TableName(){
    return "TotalWidth";
}

vector<string> TotalWidth::ColumnNames(){
    vector<string> columns;
    columns.reserve(10);
    columns.push_back("ID");
    columns.push_back("InputID");
    columns.push_back("PartialWidthIDs");
    columns.push_back("ParentID");
    columns.push_back("TotalWidth");
    columns.push_back("ScaleFactorID");
    return columns;
}

vector<string> TotalWidth::ColumnData(){
    vector<string> data;
    data.reserve(10);
    data.push_back( "'" + boost::lexical_cast<std::string>(totalWidth_.Id) + "'");
    data.push_back( "'" + boost::lexical_cast<std::string>(totalWidth_.InputId) + "'");

    // Need to properly concatenate all the uuids for associated partial widths
    if ( totalWidth_.PartialWidthIds.size() == 0 ){
        data.push_back( "NULL" );
    } else{
        string partialWidthIds;
        partialWidthIds.append("'");
        partialWidthIds.append( concatenateMultipleValues( totalWidth_.PartialWidthIds ) );
        partialWidthIds.append("'");
        data.push_back( partialWidthIds );
    }

    data.push_back( "'" + boost::lexical_cast<std::string>(totalWidth_.ParentId) + "'");
    data.push_back( boost::lexical_cast<std::string>(totalWidth_.Width) );

    if (totalWidth_.ScaleFactorId.is_nil()){
        data.push_back("NULL");
    } else{
        data.push_back( "'" + boost::lexical_cast<std::string>(totalWidth_.ScaleFactorId) + "'");
    }

    return data;
}

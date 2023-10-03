#include <sql/Statements/DeltaNeff.h>

using namespace std;
using namespace Statements;

DeltaNeff::DeltaNeff(const Models::DeltaNeff& deltaNeff, StatementType statementType) : 
    IStatement(statementType)
{
    deltaNeff_ = deltaNeff;
}

DeltaNeff::~DeltaNeff()
{
}

void DeltaNeff::Validate(){
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

void DeltaNeff::validateCreate(){
    if ( deltaNeff_.Id.is_nil()){
        throw_with_trace( logic_error("Invalid Id") );
    }
    if ( deltaNeff_.InputId.is_nil() ){
        throw_with_trace( logic_error("Invalid InputId") );
    }
    if ( deltaNeff_.Delta_Neff < 0. ){
        throw_with_trace( logic_error("Invalid DeltaNeff") );
    }
}

string DeltaNeff::TableName(){
    return "DeltaNeff";
}

vector<string> DeltaNeff::ColumnNames(){
    vector<string> columns;
    columns.reserve(10);
    columns.push_back("ID");
    columns.push_back("InputID");
    columns.push_back("ParentID");
    columns.push_back("ChildrenIDs");
    columns.push_back("ParticleID");
    columns.push_back("ParticleEvolutionID");
    columns.push_back("DeltaNeff");
    columns.push_back("ScaleFactorID");
    return columns;
}

vector<string> DeltaNeff::ColumnData(){
    vector<string> data;
    data.reserve(10);
    data.push_back( "'" + boost::lexical_cast<std::string>(deltaNeff_.Id) + "'");
    data.push_back( "'" + boost::lexical_cast<std::string>(deltaNeff_.InputId) + "'");
    if ( deltaNeff_.ParentId.is_nil() ){
        data.push_back("NULL");
    } else{
        data.push_back( "'" + boost::lexical_cast<std::string>(deltaNeff_.ParentId) + "'");
    }

    if ( deltaNeff_.ChildrenIds.size() > 0 ){
        // Need to properly concatenate all the uuids for children
        string childrenIds;
        childrenIds.append("'");
        for (auto c = deltaNeff_.ChildrenIds.begin(); c != deltaNeff_.ChildrenIds.end(); ++c){
            childrenIds.append("{");
            childrenIds.append( concatenateMultipleValues( *c ) );
            childrenIds.append("}");
            if (std::next(c) != deltaNeff_.ChildrenIds.end()){
                childrenIds.append(", ");
            }
        }  
        childrenIds.append("'");
        data.push_back( childrenIds );
    } else{
        data.push_back("NULL");
    }

    if (deltaNeff_.ParticleId.is_nil()){
        data.push_back("NULL");
    } else{
        data.push_back( "'" + boost::lexical_cast<std::string>(deltaNeff_.ParticleId) + "'");
    }

    if (deltaNeff_.ParticleEvolutionId.is_nil()){
        data.push_back("NULL");
    } else{
        data.push_back( "'" + boost::lexical_cast<std::string>(deltaNeff_.ParticleEvolutionId) + "'");
    }

    data.push_back( boost::lexical_cast<std::string>(deltaNeff_.Delta_Neff) );

    if (deltaNeff_.ScaleFactorId.is_nil()){
        data.push_back("NULL");
    } else{
        data.push_back( "'" + boost::lexical_cast<std::string>(deltaNeff_.ScaleFactorId) + "'");
    }
    
    return data;
}
 
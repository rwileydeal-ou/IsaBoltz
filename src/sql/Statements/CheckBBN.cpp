#include <sql/Statements/CheckBBN.h>

using namespace std;
using namespace Statements;

CheckBBN::CheckBBN(const Models::CheckBBN& checkBBN, StatementType statementType) : 
    IStatement(statementType),
    checkBBN_(checkBBN)
{
}

CheckBBN::~CheckBBN()
{
}

void CheckBBN::Validate(){
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
 
void CheckBBN::validateCreate(){
    if ( checkBBN_.Id.is_nil()){
        throw_with_trace( logic_error("Invalid Id") );
    }
    if ( checkBBN_.InputId.is_nil() ){
        throw_with_trace( logic_error("Invalid InputId") );
    }
    if ( checkBBN_.ParticleId.is_nil() ){
        throw_with_trace( logic_error("Invalid ParticleId") );
    }
    if ( checkBBN_.ParticleEvolutionId.is_nil() ){
        throw_with_trace( logic_error("Invalid ParticleEvolutionId") );
    }
    if ( checkBBN_.BrHadronic < 0. ){
        throw_with_trace( logic_error("Invalid BrHadronic") );
    }
}

string CheckBBN::TableName(){
    return "CheckBBN";
}

vector<string> CheckBBN::ColumnNames(){
    vector<string> columns;
    columns.reserve(10);
    columns.push_back("ID");
    columns.push_back("InputID");
    columns.push_back("ParticleID");
    columns.push_back("ParticleEvolutionID");
    columns.push_back("BrHadronic");
    columns.push_back("DensityConstraint");
    columns.push_back("DensityCalculated");
    columns.push_back("ConstraintSatisfied");
    columns.push_back("ScaleFactorID");
    return columns;
}

vector<string> CheckBBN::ColumnData(){
    vector<string> data;
    data.reserve(10);
    data.push_back( "'" + boost::lexical_cast<std::string>(checkBBN_.Id) + "'");
    data.push_back( "'" + boost::lexical_cast<std::string>(checkBBN_.InputId) + "'");
    data.push_back( "'" + boost::lexical_cast<std::string>(checkBBN_.ParticleId) + "'");
    data.push_back( "'" + boost::lexical_cast<std::string>(checkBBN_.ParticleEvolutionId) + "'");
    data.push_back( numericGuard(checkBBN_.BrHadronic) );
    data.push_back( numericGuard(checkBBN_.DensityConstraint) );
    data.push_back( numericGuard(checkBBN_.DensityCalculated) );
    data.push_back( numericGuard(checkBBN_.ConstraintSatisfied) );

    if (checkBBN_.ScaleFactorId.is_nil()){
        data.push_back("NULL");
    } else{
        data.push_back( "'" + boost::lexical_cast<std::string>(checkBBN_.ScaleFactorId) + "'");
    }

    return data;
}

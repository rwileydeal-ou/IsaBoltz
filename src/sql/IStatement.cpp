#include <sql/IStatement.h>

using namespace std;
using namespace Statements;

IStatement::IStatement(StatementType statementType){
    statementType_ = statementType;
}
IStatement::~IStatement(){
}

string IStatement::Statement(){
    Validate();
    switch (statementType_) {
        case Create:
            return processCreate();
        case Read:
            return processRead();
        case Delete:
            return processDelete();
        case Update:
            return processUpdate();
        default:
            throw NotImplementedException();
    }
}

std::string IStatement::numericGuard( double value ){
    if (std::isinf( value ) || std::isnan( value )){
        return "NULL";
    }
    return boost::lexical_cast<std::string>(value);
}

string IStatement::processCreate(){
    string statement = "INSERT INTO ";
    statement.append( TableName() );
    statement.append( " " );

    // here we outline the table
    statement.append( "(" );
    auto colNames = ColumnNames();
    statement.append( concatenateMultipleValues( colNames ) );
    statement.append( ")" );

    // now we append the values 
    statement.append(" VALUES ");
    statement.append( "(" );
    auto colData = ColumnData();
    statement.append( concatenateMultipleValues( colData ) );
    statement.append( ")" );
    return statement;
}

string IStatement::processRead(){
    string statement = "SELECT ";
    auto colNames = ColumnNames();
    statement.append( concatenateMultipleValues( colNames ) );
    statement.append(" FROM ");
    statement.append( TableName() );

    statement.append( processFilters() );
    return statement;
}

string IStatement::processDelete(){
    string statement = "DELETE FROM ";
    statement.append( TableName() );
    statement.append( processFilters() );
    return statement;
}

string IStatement::processUpdate(){
    string statement = "UPDATE ";
    statement.append( TableName() );

    statement.append(" SET ");
    for (size_t i = 0; i < ColumnNames().size(); ++i){
        statement.append( ColumnNames()[i] );
        statement.append( " = " );
        statement.append( ColumnData()[i] );
        if ( i < ColumnNames().size() - 1 ){
            statement.append(", ");
        }
    }

    statement.append( processFilters() );
    return statement;
}

// CURRENTLY ONLY SUPPORTING "AND" FILTERS
// CAN'T SEE A NEED FOR "OR" FILTER IN THIS PROJECT, AS OF YET
string IStatement::processFilters(){
    string filterString;
    if (filters.size() > 0){
        filterString.append(" WHERE (");
        for (auto f = filters.begin(); f != filters.end(); ++f){
            filterString.append( *f );
            if (std::next(f) != filters.end()){
                filterString.append(" AND ");
            }
        }
        filterString.append(")");
    }
    return filterString; 
}

void IStatement::AddFilter( Filters::IFilter& filter ){
    filters.push_back( filter.Filter() );
}

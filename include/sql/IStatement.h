#ifndef IStatement_h
#define IStatement_h

#include <string>
#include <vector>
#include <deque>
#include <iterator>
#include <boost/lexical_cast.hpp>
#include <io/Exceptions/NotImplemented.h>
#include <sql/IFilter.h>

namespace Statements{
    enum StatementType{
        Create,
        Read,
        Update,
        Delete
    };

    class IStatement
    {
    private:
        std::vector<std::string> filters;
        std::string processFilters();
        std::string processCreate();
        std::string processRead();
        std::string processUpdate();
        std::string processDelete();
    protected:
        template < typename T, typename A >
        std::string concatenateMultipleValues( std::vector<T,A> const& values ){
            std::string statement;
            for ( auto value = values.begin(); value != values.end(); ++value ){
                statement.append( boost::lexical_cast< std::string > ( *value ) );
                if (std::next(value) != values.end()){
                    statement.append(", ");
                }
            }
            return statement;
        }
        template < typename T, typename A >
        std::string concatenateMultipleValues( std::deque<T,A> const& values ){
            std::string statement;
            for ( auto value = values.begin(); value != values.end(); ++value ){
                statement.append( boost::lexical_cast< std::string > ( *value ) );
                if (std::next(value) != values.end()){
                    statement.append(", ");
                }
            }
            return statement;
        }
        StatementType statementType_;
        virtual std::string TableName() = 0;
        virtual std::vector<std::string> ColumnNames() = 0;
        virtual std::vector<std::string> ColumnData() = 0;
        std::string numericGuard( double value );
    public:
        std::string Statement();
        virtual void Validate() = 0;
        void AddFilter( Filters::IFilter& filter );
        IStatement(StatementType statementType);
        ~IStatement();
    };
}

#endif

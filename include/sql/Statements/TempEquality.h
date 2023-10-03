#ifndef StatementsTempEquality_h
#define StatementsTempEquality_h

#include <sql/IStatement.h>
#include <Models/TempEquality.h>
#include <io/Logging/Logger.h>
#include <boost/lexical_cast.hpp>

namespace Statements{
    class TempEquality : public IStatement
    {
    private:
        Models::TempEquality tempEquality_;
        std::string TableName() override;
        std::vector<std::string> ColumnNames() override;
        std::vector<std::string> ColumnData() override;
        void validateCreate();
    public:
        void Validate() override;
        TempEquality(const Models::TempEquality& tempEquality, StatementType statementType);
        ~TempEquality();
    };
}

#endif

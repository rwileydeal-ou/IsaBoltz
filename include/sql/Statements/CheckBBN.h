#ifndef StatementsCheckBBN_h
#define StatementsCheckBBN_h

#include <sql/IStatement.h>
#include <Models/CheckBBN.h>
#include <io/Logging/Logger.h>
#include <boost/lexical_cast.hpp>

namespace Statements{
    class CheckBBN : public IStatement
    {
    private:
        Models::CheckBBN checkBBN_;
        std::string TableName() override;
        std::vector<std::string> ColumnNames() override;
        std::vector<std::string> ColumnData() override;
        void validateCreate();
    public:
        void Validate() override;
        CheckBBN(const Models::CheckBBN& checkBBN, StatementType statementType);
        ~CheckBBN();
    };
}

#endif

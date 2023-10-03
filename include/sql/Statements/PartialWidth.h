#ifndef StatementsPartialWidth_h
#define StatementsPartialWidth_h

#include <sql/IStatement.h>
#include <Models/PartialWidth.h>
#include <io/Logging/Logger.h>
#include <boost/lexical_cast.hpp>

namespace Statements{
    class PartialWidth : public IStatement
    {
    private:
        Models::PartialWidth partialWidth_;
        std::string TableName() override;
        std::vector<std::string> ColumnNames() override;
        std::vector<std::string> ColumnData() override;
        void validateCreate();
    public:
        void Validate() override;
        PartialWidth(const Models::PartialWidth& partialWidth, StatementType statementType);
        ~PartialWidth();
    };
}

#endif

#ifndef StatementsTotalWidth_h
#define StatementsTotalWidth_h

#include <sql/IStatement.h>
#include <Models/TotalWidth.h>
#include <io/Logging/Logger.h>
#include <boost/lexical_cast.hpp>

namespace Statements{
    class TotalWidth : public IStatement
    {
    private:
        Models::TotalWidth totalWidth_;
        std::string TableName() override;
        std::vector<std::string> ColumnNames() override;
        std::vector<std::string> ColumnData() override;
        void validateCreate();
    public:
        void Validate() override;
        TotalWidth(const Models::TotalWidth& totalWidth, StatementType statementType);
        ~TotalWidth();
    };
}

#endif

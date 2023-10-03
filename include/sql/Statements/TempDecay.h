#ifndef StatementsTempDecay_h
#define StatementsTempDecay_h

#include <sql/IStatement.h>
#include <Models/TempDecay.h>
#include <io/Logging/Logger.h>
#include <boost/lexical_cast.hpp>

namespace Statements{
    class TempDecay : public IStatement
    {
    private:
        Models::TempDecay tempDecay_;
        std::string TableName() override;
        std::vector<std::string> ColumnNames() override;
        std::vector<std::string> ColumnData() override;
        void validateCreate();
    public:
        void Validate() override;
        TempDecay(const Models::TempDecay& tempDecay, StatementType statementType);
        ~TempDecay();
    };
}

#endif

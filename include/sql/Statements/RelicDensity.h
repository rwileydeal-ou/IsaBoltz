#ifndef StatementsRelicDensity_h
#define StatementsRelicDensity_h

#include <sql/IStatement.h>
#include <Models/RelicDensity.h>
#include <io/Logging/Logger.h>
#include <boost/lexical_cast.hpp>

namespace Statements{
    class RelicDensity : public IStatement
    {
    private:
        Models::RelicDensity relicDensity_;
        std::string TableName() override;
        std::vector<std::string> ColumnNames() override;
        std::vector<std::string> ColumnData() override;
        void validateCreate();
    public:
        void Validate() override;
        RelicDensity(const Models::RelicDensity& relicDensity, StatementType statementType);
        ~RelicDensity();
    };
}

#endif

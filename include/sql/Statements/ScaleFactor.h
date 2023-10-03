#ifndef StatementsScaleFactor
#define StatementsScaleFactor

#include <sql/IStatement.h>
#include <Models/ScaleFactor.h>
#include <io/Logging/Logger.h>
#include <boost/lexical_cast.hpp>

namespace Statements{
    class ScaleFactor : public IStatement
    {
    private:
        Models::ScaleFactorPoint scaleFactor_;
        std::string TableName() override;
        std::vector<std::string> ColumnNames() override;
        std::vector<std::string> ColumnData() override;
        void validateCreate();
    public:
        void Validate() override;
        ScaleFactor(const Models::ScaleFactorPoint& scaleFactor, StatementType statementType);
        ~ScaleFactor();
    };
}

#endif

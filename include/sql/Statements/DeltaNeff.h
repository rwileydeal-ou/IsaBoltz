#ifndef StatementsDeltaNeff_h
#define StatementsDeltaNeff_h

#include <sql/IStatement.h>
#include <Models/DeltaNeff.h>
#include <io/Logging/Logger.h>
#include <boost/lexical_cast.hpp>

namespace Statements{
    class DeltaNeff : public IStatement
    {
    private:
        Models::DeltaNeff deltaNeff_;
        std::string TableName() override;
        std::vector<std::string> ColumnNames() override;
        std::vector<std::string> ColumnData() override;
        void validateCreate();
    public:
        void Validate() override;
        DeltaNeff(const Models::DeltaNeff& deltaNeff, StatementType statementType);
        ~DeltaNeff();
    };
}

#endif

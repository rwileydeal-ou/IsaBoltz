#ifndef StatementsTempOsc_h
#define StatementsTempOsc_h

#include <sql/IStatement.h>
#include <Models/TempOsc.h>
#include <io/Logging/Logger.h>
#include <boost/lexical_cast.hpp>

namespace Statements{
    class TempOsc : public IStatement
    {
    private:
        Models::TempOscillation tempOsc_;
        std::string TableName() override;
        std::vector<std::string> ColumnNames() override;
        std::vector<std::string> ColumnData() override;
        void validateCreate();
    public:
        void Validate() override;
        TempOsc(const Models::TempOscillation& tempOsc, StatementType statementType);
        ~TempOsc();
    };
}

#endif

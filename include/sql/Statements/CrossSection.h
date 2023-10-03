#ifndef StatementsCrossSection_h
#define StatementsCrossSection_h

#include <sql/IStatement.h>
#include <ModelIndependent/CrossSections/SigmaV.h>
#include <io/Logging/Logger.h>
#include <boost/lexical_cast.hpp>

namespace Statements{
    class CrossSection : public IStatement
    {
    private:
        SigmaV crossSection_;
        std::string TableName() override;
        std::vector<std::string> ColumnNames() override;
        std::vector<std::string> ColumnData() override;
        void validateCreate();
    public:
        void Validate() override;
        CrossSection(const SigmaV& crossSection, StatementType statementType);
        ~CrossSection();
    };
}

#endif

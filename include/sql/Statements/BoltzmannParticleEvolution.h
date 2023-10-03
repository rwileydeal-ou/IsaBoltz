#ifndef StatementsBoltzmannParticleEvolution_h
#define StatementsBoltzmannParticleEvolution_h

#include <sql/IStatement.h>
#include <Models/ParticleEvolution.h>
#include <io/Logging/Logger.h>
#include <boost/lexical_cast.hpp>

namespace Statements{
    class BoltzmannParticleEvolution : public IStatement
    {
    private:
        Models::ParticleEvolution boltzmannParticleEvolution_;
        std::string TableName() override;
        std::vector<std::string> ColumnNames() override;
        std::vector<std::string> ColumnData() override;
        void validateCreate();
    public:
        void Validate() override;
        BoltzmannParticleEvolution(const Models::ParticleEvolution& particleEvolution, StatementType statementType);
        ~BoltzmannParticleEvolution();
    };
}

#endif

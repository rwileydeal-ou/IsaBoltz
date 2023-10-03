#ifndef StatementsParticle_h
#define StatementsParticle_h

#include <sql/IStatement.h>
#include <Models/Particle.h>
#include <io/Logging/Logger.h>
#include <boost/lexical_cast.hpp>

namespace Statements{
    class Particle : public IStatement
    {
    private:
        Models::Particle particle_;
        std::string TableName() override;
        std::vector<std::string> ColumnNames() override;
        std::vector<std::string> ColumnData() override;
        void validateCreate();
    public:
        void Validate() override;
        Particle(const Models::Particle& particle, StatementType statementType);
        ~Particle();
    };
}

#endif

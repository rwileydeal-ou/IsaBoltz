#ifndef StatementsInputs_h
#define StatementsInputs_h

#include <sql/IStatement.h>
#include <Models/Connection.h>
#include <io/Logging/Logger.h>
#include <boost/lexical_cast.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/optional.hpp>

namespace Statements{
    class Inputs : public IStatement
    {
    private:
        const Connection& connection_;
        std::string TableName() override;
        std::vector<std::string> ColumnNames() override;
        std::vector<std::string> ColumnData() override;
        void validateCreate();
        void writeNuhm3Params( boost::property_tree::ptree& pt );
        void writeDfszParams( boost::property_tree::ptree& pt );
        void writeModulusParams( boost::property_tree::ptree& pt );
        std::string handleModelParams();
        std::string handleCosmologyParams();
    public:
        void Validate() override;
        Inputs(const Connection& connection, StatementType statementType);
        ~Inputs();
    };
}

#endif

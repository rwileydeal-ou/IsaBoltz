#include <sql/Statements/Particle.h>

using namespace std;
using namespace Statements;

Particle::Particle(const Models::Particle& particle, StatementType statementType) :
    IStatement(statementType)
{
    particle_ = particle;
}

Particle::~Particle()
{
}

void Particle::Validate(){
    switch (statementType_)
    {
        case Create:
            validateCreate();
            break;
        case Read:
            break;
        default:
            throw_with_trace( NotImplementedException() );
    }
}

void Particle::validateCreate(){
    if ( particle_.Id.is_nil()){
        throw_with_trace( logic_error("Invalid Id") );
    }
    if ( particle_.InputId.is_nil() ){
        throw_with_trace( logic_error("Invalid InputId") );
    }
    if ( particle_.Key.empty() ){
        throw_with_trace( logic_error("Invalid Key") );
    }
    if ( particle_.Name.empty() ){
        throw_with_trace( logic_error("Invalid Name") );
    }
    if ( particle_.PdgCode <= 0 ){
        throw_with_trace( logic_error("Invalid PdgCode") );
    }
}

string Particle::TableName(){
    return "Particle";
}

vector<string> Particle::ColumnNames(){
    vector<string> columns;
    columns.reserve(10);
    columns.push_back("ID");
    columns.push_back("InputID");
    columns.push_back("ParticleKey");
    columns.push_back("Name");
    columns.push_back("Pdg");
    columns.push_back("Mass");
    columns.push_back("Spin");
    columns.push_back("Charges");
    columns.push_back("ScaleFactorID");
    return columns;
}

vector<string> Particle::ColumnData(){
    vector<string> data;
    data.reserve(10);
    data.push_back("'" + boost::lexical_cast<string>(particle_.Id) + "'");
    data.push_back("'" + boost::lexical_cast<string>(particle_.InputId) + "'");
    data.push_back("'" + particle_.Key + "'");
    data.push_back("'" + particle_.Name + "'");
    data.push_back(boost::lexical_cast<string>(particle_.PdgCode));
    data.push_back(boost::lexical_cast<string>(particle_.Mass));
    data.push_back(boost::lexical_cast<string>(particle_.Spin));


    auto rparity = *find_if( particle_.Charges.begin(), particle_.Charges.end(), 
        [&] (Models::Charge const& c) {return (c.Group == Models::GaugeGroup::RParity); });

    std::string chargeJSON;
    chargeJSON.append("'{");
    chargeJSON.append("\"charges\":[");

    // just do r-parity for now, TODO: revise this!
    chargeJSON.append("{");
    chargeJSON.append("\"key\":\"rparity\",");
    chargeJSON.append("\"value\":" + boost::lexical_cast<string>( rparity.Value ));
    chargeJSON.append("}");

    chargeJSON.append("]");
    chargeJSON.append("}'");
    data.push_back( chargeJSON );

    if ( particle_.ScaleFactorId.is_nil() ){
        data.push_back("NULL");
    } else{
        data.push_back("'" + boost::lexical_cast<string>(particle_.ScaleFactorId) + "'");
    }
    return data;
}
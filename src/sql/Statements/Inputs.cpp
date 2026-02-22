#include <sql/Statements/Inputs.h>

using namespace std;
using namespace Statements;

Inputs::Inputs(const Connection& connection, StatementType statementType) : 
    IStatement(statementType),
    connection_(connection)
{
}

Inputs::~Inputs()
{
}

void Inputs::Validate(){
    switch (statementType_)
    {
        case Create:
            validateCreate();
            break;
        case Read:
            break;
        case Delete:
            break;
        default:
            throw_with_trace( NotImplementedException() );
    }
}

void Inputs::validateCreate(){
    if ( connection_.InputId.is_nil()){
        throw_with_trace( logic_error("Invalid Id") );
    }
}

string Inputs::TableName(){
    return "Inputs";
}

vector<string> Inputs::ColumnNames(){
    vector<string> columns;
    columns.reserve(10);
    columns.push_back("ID");
    columns.push_back("ModelParams");
    columns.push_back("CosmologyParams");
    return columns;
}

void Inputs::writeNuhm3Params( boost::property_tree::ptree& pt ){
    pt.put("Model.Nuhm3Params.M_0", boost::lexical_cast<std::string>( connection_.Model.Masses.M0 ) );
    pt.put("Model.Nuhm3Params.M_Hf", boost::lexical_cast<std::string>( connection_.Model.Masses.MHF ) );
    pt.put("Model.Nuhm3Params.A0", boost::lexical_cast<std::string>( connection_.Model.A0 ) );
    pt.put("Model.Nuhm3Params.TanBeta", boost::lexical_cast<std::string>( connection_.Model.Susy.TanBeta ) );
    pt.put("Model.Nuhm3Params.SgnMu", boost::lexical_cast<std::string>( connection_.Model.SgnMu ) );
    pt.put("Model.Nuhm3Params.mu_Weak", boost::lexical_cast<std::string>( connection_.Model.Options.NonUniversalSugra.WeakInput.weakMu ) );
    pt.put("Model.Nuhm3Params.M_A_Weak", boost::lexical_cast<std::string>( connection_.Model.Options.NonUniversalSugra.WeakInput.weakMA ) );
    pt.put("Model.Nuhm3Params.uL_GUT", boost::lexical_cast<std::string>( connection_.Model.Options.NonUniversalSugra.GenOneTwo.m_uL_GUT ) );
    pt.put("Model.Nuhm3Params.dR_GUT", boost::lexical_cast<std::string>( connection_.Model.Options.NonUniversalSugra.GenOneTwo.m_dR_GUT ) );
    pt.put("Model.Nuhm3Params.uR_GUT", boost::lexical_cast<std::string>( connection_.Model.Options.NonUniversalSugra.GenOneTwo.m_uR_GUT ) );
    pt.put("Model.Nuhm3Params.eL_GUT", boost::lexical_cast<std::string>( connection_.Model.Options.NonUniversalSugra.GenOneTwo.m_eL_GUT ) );
    pt.put("Model.Nuhm3Params.eR_GUT", boost::lexical_cast<std::string>( connection_.Model.Options.NonUniversalSugra.GenOneTwo.m_eR_GUT ) );
    pt.put("Model.Nuhm3Params.LambdaRPV", boost::lexical_cast<std::string>( connection_.Model.Options.NonUniversalSugra.LambdaRPV ) );
}

void Inputs::writeDfszParams( boost::property_tree::ptree& pt ){
    pt.put("Model.DfszParams.ThetaI", boost::lexical_cast<std::string>( connection_.Model.PQSector.Theta_I ) );
    pt.put("Model.DfszParams.Fa", boost::lexical_cast<std::string>( connection_.Model.PQSector.Fa ) );
    pt.put("Model.DfszParams.Xi", boost::lexical_cast<std::string>( connection_.Model.PQSector.Xi ) );
    auto sax = ModelBaseOps::FindConst( connection_.Model.Particles, "saxion" );
    pt.put("Model.DfszParams.Saxion.Amplitude", boost::lexical_cast<std::string>( sax.CohOscComponents.Amplitude ) );
    pt.put("Model.DfszParams.Saxion.Mass", boost::lexical_cast<std::string>( sax.Mass ) );
    auto ax = ModelBaseOps::FindConst( connection_.Model.Particles, "axino" );
    pt.put("Model.DfszParams.Axino.Mass", boost::lexical_cast<std::string>( ax.Mass ) );
}

void Inputs::writeModulusParams( boost::property_tree::ptree& pt ){
    auto mod = ModelBaseOps::FindConst( connection_.Model.Particles, "modulus" );
    pt.put("Model.ModulusParams.Mass", boost::lexical_cast<std::string>( mod.Mass ) );
    pt.put("Model.ModulusParams.Amplitude", boost::lexical_cast<std::string>( mod.CohOscComponents.Amplitude ) );
    pt.put("Model.ModulusParams.LambdaG", boost::lexical_cast<std::string>( connection_.Model.ModulusCouplings.GravitinoCouplings.LambdaG ) );
    std::stringstream ss;
    ss << connection_.Model.ModulusCouplings.GaugeCouplings.LambdaU1;
    std::string testPrecision = ss.str();    
    pt.put("Model.ModulusParams.LambdaU1", testPrecision );
    pt.put("Model.ModulusParams.LambdaSU2", boost::lexical_cast<std::string>( connection_.Model.ModulusCouplings.GaugeCouplings.LambdaSU2 ) );
    pt.put("Model.ModulusParams.LambdaSU3", boost::lexical_cast<std::string>( connection_.Model.ModulusCouplings.GaugeCouplings.LambdaSU3 ) );
    pt.put("Model.ModulusParams.LambdaHGM", boost::lexical_cast<std::string>( connection_.Model.ModulusCouplings.HiggsCouplings.LambdaGM ) );
    pt.put("Model.ModulusParams.LambdaHu", boost::lexical_cast<std::string>( connection_.Model.ModulusCouplings.HiggsCouplings.LambdaHu ) );
    pt.put("Model.ModulusParams.LambdaHd", boost::lexical_cast<std::string>( connection_.Model.ModulusCouplings.HiggsCouplings.LambdaHd ) );
    pt.put("Model.ModulusParams.LambdaUu", boost::lexical_cast<std::string>( connection_.Model.ModulusCouplings.QuarkSquarkCouplings.LambdaUu ) );
    pt.put("Model.ModulusParams.LambdaUc", boost::lexical_cast<std::string>( connection_.Model.ModulusCouplings.QuarkSquarkCouplings.LambdaUc ) );
    pt.put("Model.ModulusParams.LambdaUt", boost::lexical_cast<std::string>( connection_.Model.ModulusCouplings.QuarkSquarkCouplings.LambdaUt ) );
    pt.put("Model.ModulusParams.LambdaDd", boost::lexical_cast<std::string>( connection_.Model.ModulusCouplings.QuarkSquarkCouplings.LambdaDd ) );
    pt.put("Model.ModulusParams.LambdaDs", boost::lexical_cast<std::string>( connection_.Model.ModulusCouplings.QuarkSquarkCouplings.LambdaDs ) );
    pt.put("Model.ModulusParams.LambdaDb", boost::lexical_cast<std::string>( connection_.Model.ModulusCouplings.QuarkSquarkCouplings.LambdaDb ) );
    pt.put("Model.ModulusParams.LambdaQ1", boost::lexical_cast<std::string>( connection_.Model.ModulusCouplings.QuarkSquarkCouplings.LambdaQ1 ) );
    pt.put("Model.ModulusParams.LambdaQ2", boost::lexical_cast<std::string>( connection_.Model.ModulusCouplings.QuarkSquarkCouplings.LambdaQ2 ) );
    pt.put("Model.ModulusParams.LambdaQ3", boost::lexical_cast<std::string>( connection_.Model.ModulusCouplings.QuarkSquarkCouplings.LambdaQ3 ) );
    pt.put("Model.ModulusParams.LambdaLe", boost::lexical_cast<std::string>( connection_.Model.ModulusCouplings.LeptonSleptonCouplings.LambdaLe ) );
    pt.put("Model.ModulusParams.LambdaLm", boost::lexical_cast<std::string>( connection_.Model.ModulusCouplings.LeptonSleptonCouplings.LambdaLm ) );
    pt.put("Model.ModulusParams.LambdaLt", boost::lexical_cast<std::string>( connection_.Model.ModulusCouplings.LeptonSleptonCouplings.LambdaLt ) );
    pt.put("Model.ModulusParams.LambdaE1", boost::lexical_cast<std::string>( connection_.Model.ModulusCouplings.LeptonSleptonCouplings.LambdaE1 ) );
    pt.put("Model.ModulusParams.LambdaE2", boost::lexical_cast<std::string>( connection_.Model.ModulusCouplings.LeptonSleptonCouplings.LambdaE2 ) );
    pt.put("Model.ModulusParams.LambdaE3", boost::lexical_cast<std::string>( connection_.Model.ModulusCouplings.LeptonSleptonCouplings.LambdaE3 ) );
    pt.put("Model.ModulusParams.LambdaAxion", boost::lexical_cast<std::string>( connection_.Model.ModulusCouplings.AxionicCouplings.LambdaAx ) );
    pt.put("Model.ModulusParams.LambdaAlp", boost::lexical_cast<std::string>( connection_.Model.ModulusCouplings.AxionicCouplings.LambdaALP ) );
}

std::string Inputs::handleModelParams(){
    std::string json;
    // TODO: ONCE CONCEPT OF A ``MODEL'' IS DEFINED, NEED TO REDO THIS!
    boost::property_tree::ptree pt;

    pt.put("Model.Name", "nuhm3_dfsz_modulus");
    writeNuhm3Params( pt );
    writeDfszParams( pt );
    writeModulusParams( pt );

    std::stringstream ss;
    boost::property_tree::json_parser::write_json(ss, pt);
    json = ss.str();
    return json;
}

std::string Inputs::handleCosmologyParams(){
    std::string json;
    boost::property_tree::ptree pt;

    pt.put("Temperature.Reheat", connection_.Model.Cosmology.Temperatures.Reheat);

    std::stringstream ss;
    boost::property_tree::json_parser::write_json(ss, pt);
    json = ss.str();
    return json;
}

vector<string> Inputs::ColumnData(){
    vector<string> data;
    data.reserve(10);
    data.push_back( "'" + boost::lexical_cast<std::string>(connection_.InputId) + "'");
    data.push_back( "'" + handleModelParams() + "'");
    data.push_back( "'" + handleCosmologyParams() + "'");
    return data;
}

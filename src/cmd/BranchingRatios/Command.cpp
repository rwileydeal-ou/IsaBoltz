#include <cmd/BranchingRatios/Command.h>

using namespace std;

BranchingRatioCommand::BranchingRatioCommand(
    Connection& connection, 
    DbManager& db,
    Models::Particle& particle, 
    const std::vector< Models::Particle >& particles, 
    bool squashLogs
) :
    connection_(connection),
    db_(db),
    particle_(particle),
    particles_(particles)
{
    postResult_ = true;
    squashLogs_ = squashLogs;
    this -> receiver_ = std::make_shared< BranchingRatioReceiver >( connection_, particle_, particles_ );
}
BranchingRatioCommand::~BranchingRatioCommand(){
}

void BranchingRatioCommand::postSqlTotalWidth(
    BranchingFraction& result
){
    auto statement = Statements::TotalWidth( result.TotalWidth, Statements::StatementType::Create );
    db_.Execute( statement );
}

void BranchingRatioCommand::postSqlPartialWidths(
    BranchingFraction& result
){
    for (auto& width : result.PartialWidths){
        width.TotalWidthId = result.TotalWidth.Id;
        auto statement = Statements::PartialWidth( width, Statements::StatementType::Create );
        db_.Execute( statement );
    } 
}

void BranchingRatioCommand::postResult(){
    auto result = receiver_ -> getBranchingFraction();
    postSqlTotalWidth(result);
    postSqlPartialWidths(result);
}

void BranchingRatioCommand::Execute(){
    this -> receiver_ -> Calculate();

    if ( postResult_ ){
        postResult();
    }

    if ( !squashLogs_ ){
        ostringstream logEntry;
        auto a = receiver_ -> getBranchingFraction();
        logEntry << "Decay width for " << particle_.Key << ": " << a.TotalWidth.Width << " GeV";
        connection_.Log.Info( logEntry.str() );
    }
}

BranchingFraction BranchingRatioCommand::getResult(){
    return receiver_ -> getBranchingFraction();
}

void BranchingRatioCommand::PostResult(bool post){
    postResult_ = post;
}

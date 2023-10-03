#include <cmd/IsaTools/Receiver.h>

IsaToolsReceiver::IsaToolsReceiver(IsaTool tool, Connection& connection, std::shared_ptr< DataRelay > fortranInterface, bool squashLogs) :
    connection_(connection)
{
    tool_ = tool;
    fortranInterface_ = fortranInterface;
    squashLogs_ = squashLogs;
    isaVal_ = 0;
    // TODO: this probably needs to get reset per model -> check!!!
    iModel_ = 1;
}
IsaToolsReceiver::~IsaToolsReceiver(){
}

void IsaToolsReceiver::Calculate(){
    switch (tool_)
    {
        case IsaTool::IsaAmu:
            runIsaAmu();
            break;
        case IsaTool::IsaBsg:
            runIsaBsg();
            break;
        case IsaTool::IsaBtn:
            runIsaBtn();
            break;
        case IsaTool::IsaRed:
            runIsaRed();
            break;
        case IsaTool::IsaRes:
            runIsaRes();
            break;    
        default:
            throw NotImplementedException();
    }
}

void IsaToolsReceiver::runIsaAmu(){
    double isaValDbl = 0;
    if (!squashLogs_){
        connection_.Log.Info("Running IsaAmu...");
    }
    fortranInterface_ -> RunIsaAmu(isaVal_, isaValDbl);
}

void IsaToolsReceiver::runIsaBsg(){
    if (!squashLogs_){
        connection_.Log.Info("Running IsaBsg...");
    }
    fortranInterface_ -> RunIsaBsg(isaVal_, iModel_, connection_.Model);
}

void IsaToolsReceiver::runIsaBtn(){
    if (!squashLogs_){
        connection_.Log.Info("Running IsaBtn...");
    }
    fortranInterface_ -> RunIsaBtn(connection_.Model);
}

void IsaToolsReceiver::runIsaRes(){
    if (!squashLogs_){
        connection_.Log.Info("Running IsaRes...");
    }
    fortranInterface_ -> RunIsaRes(isaVal_);
}

void IsaToolsReceiver::runIsaRed(){
    if (!squashLogs_){
        connection_.Log.Info("Running IsaRed...");
    }
    fortranInterface_ -> RunIsaRed(isaVal_);
}

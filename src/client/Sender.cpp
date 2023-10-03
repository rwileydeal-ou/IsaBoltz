#include <client/Sender.h>

using namespace std;

Sender::Sender(bool purge){
    purge_ = purge;
    cmds_ = vector< std::shared_ptr< ICommand > >();
}
Sender::~Sender(){
    cmds_.clear();
}

void Sender::AddCommand(std::shared_ptr< ICommand > cmd){
    cmds_.push_back(cmd);
}

void Sender::InvokeProcess(){
    for (auto& cmd_ : cmds_){
        cmd_->Execute();
    }
}

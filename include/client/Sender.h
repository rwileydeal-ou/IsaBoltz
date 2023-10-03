#ifndef Sender_h
#define Sender_h

#include <memory>
#include <iostream>
#include <vector>
#include <io/Exceptions/NotImplemented.h>
#include <cmd/ICommand.h>

class Sender
{
private:
    bool purge_;
    std::vector< std::shared_ptr< ICommand > > cmds_;
public:
    void AddCommand(std::shared_ptr< ICommand > cmd);
    void InvokeProcess();
    Sender(bool purge = true);
    ~Sender();
};

#endif

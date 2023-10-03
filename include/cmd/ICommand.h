#ifndef ICommand_h
#define ICommand_h

#include <memory>
#include <cmd/IReceiver.h>

class ICommand
{
protected:
    std::shared_ptr< IReceiver > receiver_;
public:
    ICommand( std::shared_ptr< IReceiver > receiver ) 
        :  receiver_(receiver)
    {
    }
    ICommand(){
    }
    virtual ~ICommand(){
    }
    virtual void Execute() = 0;
};

#endif

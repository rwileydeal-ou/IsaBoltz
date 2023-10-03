#ifndef ModelClient_h
#define ModelClient_h

#include <string>
#include <memory>
#include <boost/filesystem.hpp>

#include <io/Exceptions/NotImplemented.h>
#include <io/Input/Params/ConfigParser.h>
#include <io/Input/Params/ParamCardParser.h>
#include <Models/SupportedModels.h>
#include <Models/ModelBase.h>
#include <Models/Connection.h>
#include <client/Sender.h>
#include <cmd/Commands.h>
#include <cmd/MssmSpectrum/Command.h>

class ModelClient
{
private:
    Connection& connection_;
    SupportedModel model_;
    ModelBase getParams();

public:
    ModelClient(Connection& connection, SupportedModel model);
    ~ModelClient();
    void Load();
};

#endif

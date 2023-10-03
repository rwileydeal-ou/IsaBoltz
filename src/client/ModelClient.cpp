#include <client/ModelClient.h>

ModelClient::ModelClient( Connection& connection, SupportedModel model ) :
    connection_(connection)
{
    model_ = model;
}

ModelClient::~ModelClient(){
}

void ModelClient::Load(){

}

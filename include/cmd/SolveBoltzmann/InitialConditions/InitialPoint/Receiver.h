#ifndef InitialPointReceiver_h
#define InitialPointReceiver_h

#include <memory>
#include <iostream>
#include <boost/property_tree/ptree.hpp>
#include <cmd/IReceiver.h>
#include <Models/ModelBase.h>
#include <Models/ScaleFactor.h>
#include <ModelIndependent/GStar.h>

class InitialPointReceiver : public IReceiver
{
private: 
    Connection& connection_;
    Models::ScaleFactorPoint initialPoint_;
    boost::property_tree::ptree& outputTree_;
public:
    ~InitialPointReceiver();
    InitialPointReceiver(Connection& connection, boost::property_tree::ptree& model_tree);
    void Calculate() override;
    Models::ScaleFactorPoint getInitialPoint();
};

#endif

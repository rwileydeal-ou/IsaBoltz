#ifndef TempDecayReceiver_h
#define TempDecayReceiver_h

#include <iostream>
#include <boost/property_tree/ptree.hpp>
#include <cmd/IReceiver.h>
#include <Models/Connection.h>
#include <Models/Particle.h>
#include <Models/TempDecay.h>
#include <Models/TotalWidth.h>
#include <ModelIndependent/GStar.h>
#include <sql/DbManager.h>
#include <sql/Statements/TotalWidth.h>
#include <sql/Callbacks/TotalWidth.h>
#include <sql/Filters/TotalWidth.h>

class TempDecayReceiver : public IReceiver
{
private: 
    Connection& connection_;
    Models::Particle particle_;
    Models::TempDecay tempDecay_;
    double gstar_;
    Models::TotalWidth pullTotalWidth(boost::uuids::uuid particleId, boost::uuids::uuid inputId);
    double suddenWidth(const Models::TotalWidth& totalWidth, double initialGuess);
public:
    ~TempDecayReceiver();
    TempDecayReceiver(Connection& connection, Models::Particle& particle);
    TempDecayReceiver(Connection& connection, boost::uuids::uuid particleId);
    void Calculate() override;
    Models::TempDecay getTempDecay();
};

#endif

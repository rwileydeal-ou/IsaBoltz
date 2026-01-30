#ifndef EstimateDeltaNeffReceiver_h
#define EstimateDeltaNeffReceiver_h

#include <iostream>
#include <cmd/DeltaNeff/Receiver.h>

#include <Models/Connection.h>
#include <Models/Particle.h>
#include <Models/PartialWidth.h>
#include <Models/TotalWidth.h>

#include <ModelIndependent/GStar.h>
#include <Models/TempOsc.h>
#include <Models/TempDecay.h>
#include <Models/TempEquality.h>
#include <Models/DeltaNeff.h>

#include <sql/DbManager.h>
#include <sql/Statements/TotalWidth.h>
#include <sql/Callbacks/TotalWidth.h>
#include <sql/Filters/TotalWidth.h>
#include <sql/Statements/PartialWidth.h>
#include <sql/Callbacks/PartialWidth.h>
#include <sql/Filters/PartialWidth.h>
#include <sql/Statements/TempDecay.h>
#include <sql/Callbacks/TempDecay.h>
#include <sql/Filters/TempDecay.h>
#include <sql/Statements/TempEquality.h>
#include <sql/Callbacks/TempEquality.h>
#include <sql/Filters/TempEquality.h>
#include <sql/Statements/TempOsc.h>
#include <sql/Callbacks/TempOsc.h>
#include <sql/Filters/TempOsc.h>

class EstimateDeltaNeffReceiver : public DeltaNeffReceiver
{
private: 
    DbManager& db_;
    Models::Particle parent_;
    std::vector< std::vector< boost::uuids::uuid > > childrenIdPairs_;

    Models::TempOscillation pullTempOsc();
    Models::TempDecay pullTempDecay();
    Models::TempEquality pullTempEquality();
    std::vector< Models::PartialWidth > pullPartialWidths();
    Models::TotalWidth pullTotalWidth();

    double calculateRhoTD(double TDecay, double gStrEnt_TD);
    double calculateDeltaNeff() override;
public:
    ~EstimateDeltaNeffReceiver();
    EstimateDeltaNeffReceiver(
        Connection& connection, 
        DbManager& db,
        const Models::ParticleEvolution& particleEvo, 
        const Models::Particle& parent, 
        std::vector< std::vector< boost::uuids::uuid > > childrenIdPairs
    );
};

#endif

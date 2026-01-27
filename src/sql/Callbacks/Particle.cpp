#include <sql/Callbacks/Particle.h>

using namespace Callbacks;
using namespace std;

Particle::Particle(/* args */)
{
}    
Particle::~Particle()
{
}

int Particle::Callback(void *data, int argc, char **argv, char **colName){
    CallbackReturnImplementation *cb = (static_cast< CallbackReturnImplementation* > (data) );

    Models::Particle particle;
    particle.Id = boost::lexical_cast<boost::uuids::uuid>(argv[0]);
    particle.InputId = boost::lexical_cast<boost::uuids::uuid>(argv[1]);
    particle.Key = boost::lexical_cast<string>(argv[2]);
    particle.Name = boost::lexical_cast<string>(argv[3]);
    particle.PdgCode = boost::lexical_cast<int>(argv[4]);
    particle.Mass = std::abs( boost::lexical_cast<double>(argv[5]) );
    particle.Spin = boost::lexical_cast<double>(argv[6]);

    // now we'll also assign the statistics for later ease of use
    int doubleSpinMod2 = (int)std::round(2. * particle.Spin) % 2;
    if (doubleSpinMod2 == 0){
        particle.Statistics = Models::ParticleStatistics::Boson;
    } else{
        particle.Statistics = Models::ParticleStatistics::Fermion;
    }

    string chargesJSON = boost::lexical_cast<string>(argv[7]);
    stringstream ss;
    ss << chargesJSON;

    boost::property_tree::ptree jsonData;
    boost::property_tree::read_json( ss, jsonData );

    // TODO: UPDATE THIS ONCE REVISED
    Models::Charge rparity;
    rparity.Group = Models::GaugeGroup::RParity;
    auto charges = jsonData.get_child("charges");
    for ( auto& c : charges ){
        rparity.Value = stof( c.second.get<string>( "value" ) );
    }
    particle.Charges.push_back( rparity );

    cb->Particles.push_back(particle);
    return 0;
}

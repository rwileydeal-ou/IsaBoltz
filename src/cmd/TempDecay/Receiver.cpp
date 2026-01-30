#include <cmd/TempDecay/Receiver.h>

TempDecayReceiver::TempDecayReceiver(
    Connection& connection, 
    DbManager& db,
    Models::Particle& particle
) :
    connection_(connection),
    db_(db)
{
    particle_ = particle;
    gstar_ = 0.;
    tempDecay_.InputId = connection_.InputId;
    tempDecay_.ParticleId = particle_.Id;
}
TempDecayReceiver::~TempDecayReceiver(){
}

// This method calculates the decay temperature in the sudden width approximation
// assumes radiation dominated universe after decay
// from Andre thesis, sec 5.2
double TempDecayReceiver::suddenWidth(const Models::TotalWidth& totalWidth, double initialGuess){
    double T1 = 0.;
    double dT = 0.;

    auto a = (initialGuess-T1) / initialGuess;
    while( (initialGuess-T1) / initialGuess > 0.01){
        double gstr = GStar::CalculateEntropic(db_, connection_, initialGuess);
        gstar_ = gstr;
        T1 = sqrt( connection_.Model.Constants.mPlanck * totalWidth.Width ) * pow( 90. / ( gstr * pow(M_PI, 2.) ), 1./4.);
        if( abs(initialGuess-T1) == dT ){
            initialGuess = (initialGuess + T1) / 2.;
        }
        else{
            dT = abs( initialGuess - T1);
            initialGuess = T1;
        }
    }
    return initialGuess;
}

Models::TotalWidth TempDecayReceiver::pullTotalWidth(boost::uuids::uuid particleId, boost::uuids::uuid inputId){
    Models::TotalWidth br;

    auto statement = Statements::TotalWidth(br, Statements::StatementType::Read);
    auto filter = Filters::TotalWidth(inputId, particleId);
    statement.AddFilter(filter);
    auto cb = Callbacks::TotalWidth();
    db_.Execute(statement, cb.Callback, cb.CallbackReturn);

    if (cb.CallbackReturn.TotalWidths.size() != 1){
        throw_with_trace( std::logic_error("Could not find unique TotalWidth") );
    }

    br = cb.CallbackReturn.TotalWidths.front();
    return br;
}

void TempDecayReceiver::Calculate(){
    auto br = pullTotalWidth(particle_.Id, connection_.InputId);
    // see e.g. arXiv 1103.5413 Eqn 5.2 -> denominator is O(1) so guess the numerator for fast convergence
    double initialGuess = sqrt( br.Width * connection_.Model.Constants.mPlanck );
    tempDecay_.Temperature = suddenWidth(br, initialGuess);
    tempDecay_.GStarEntropic = GStar::CalculateEntropic(db_, connection_, tempDecay_.Temperature);
    tempDecay_.GStar = GStar::Calculate(db_, connection_, tempDecay_.Temperature);
}

Models::TempDecay TempDecayReceiver::getTempDecay(){
    return tempDecay_;
}

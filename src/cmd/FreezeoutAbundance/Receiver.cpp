#include <cmd/FreezeoutAbundance/Receiver.h>

FreezeoutAbundanceReceiver::FreezeoutAbundanceReceiver(
    Connection& connection, 
    DbManager& db,
    Models::Particle& particle, 
    boost::uuids::uuid scaleFactorId
) :
    connection_(connection),
    db_(db),
    particle_(particle)
{
    scaleFactorId_ = scaleFactorId;
}
FreezeoutAbundanceReceiver::~FreezeoutAbundanceReceiver(){
}

void FreezeoutAbundanceReceiver::Calculate(){
    SigmaV sigV = getCrossSection();
    double gStr = GStar::Calculate(db_, connection_, sigV.Temperature);
    double gStrEnt = GStar::CalculateEntropic(db_, connection_, sigV.Temperature);

    // comes from H(Tf) ~ <sig.v> n_X (Tf)
    // assumes radiation-dominated universe
    freezeoutAbundance_ = 15. / (2. * M_PI * connection_.Model.Constants.mPlanck * sqrt( 10. ) )
        * sqrt(gStr) / ( sigV.CrossSection * gStrEnt * sigV.Temperature );

    // Just takes n = Y * s
    freezeoutNumberDensity_ = freezeoutAbundance_ * 2. * pow( M_PI, 2. ) / 45. * gStrEnt * pow( sigV.Temperature, 3. );
}

double FreezeoutAbundanceReceiver::getFreezeoutAbundance(){
    return freezeoutAbundance_;
}

double FreezeoutAbundanceReceiver::getFreezeoutNumberDensity(){
    return freezeoutNumberDensity_;
}

SigmaV FreezeoutAbundanceReceiver::getCrossSection(){
    SigmaV sigV;
    auto statement = Statements::CrossSection( sigV, Statements::Read );
    // need to add filters here!
    auto callbacks = Callbacks::CrossSection();
    db_.Execute( statement, callbacks.Callback, callbacks.CallbackReturn );
    // need to validate only pulled one cross section here!
    sigV = callbacks.CallbackReturn.crossSections.front();

/*    GetSQLSigmaV sql(connection_);
    sql.StartFilter();
    sql.AddFilterStatement("InputID", boost::lexical_cast<std::string>(connection_.InputId), SqlDataType::TEXT, SqlComparator::Equals);
    sql.And();
    sql.AddFilterStatement("ParticleID", boost::lexical_cast<std::string>(particle_.Id), SqlDataType::TEXT, SqlComparator::Equals);
    sql.And();
    sql.AddFilterStatement("ScaleFactorID", boost::lexical_cast<std::string>(scaleFactorId_), SqlDataType::TEXT, SqlComparator::Equals);
    sql.Execute();
    auto sigV = sql.get_cross_sections().front();
*/
    return sigV;
}

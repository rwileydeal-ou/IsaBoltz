#include <cmd/CriticalAbundance/Receiver.h>

CriticalAbundanceReceiver::CriticalAbundanceReceiver(Connection& connection) : 
    connection_(connection)
{
    DbManager db(connection_);
    db.Open();
    auto statement = Statements::CrossSection(crossSection_, Statements::StatementType::Read);
    auto cb = Callbacks::CrossSection();
    auto filter = Filters::CrossSection(connection_.InputId, connection_.InputId, connection_.InputId);
    statement.AddFilter( filter );
    db.Execute( statement, cb.Callback, cb.CallbackReturn );

    db.Close();

    crossSection_ = cb.CallbackReturn.crossSections.front();
}
CriticalAbundanceReceiver::~CriticalAbundanceReceiver(){
}

void CriticalAbundanceReceiver::Calculate(){
    double tempDecay = crossSection_.Temperature;
    double gStr = GStar::Calculate(connection_, tempDecay);
    double gStrEnt = GStar::CalculateEntropic(connection_, tempDecay);

    criticalAbundance_ = 45. / (2. * M_PI * connection_.Model.Constants.mPlanck * sqrt( 10. ) )
        * sqrt(gStr) / ( ( crossSection_.CrossSection ) * gStrEnt * tempDecay );
    
    criticalNumberDensity_ = criticalAbundance_ * 2. * pow( M_PI, 2. ) / 45. * gStrEnt * pow( tempDecay, 3. );
}

double CriticalAbundanceReceiver::getCriticalAbundance(){
    return criticalAbundance_;
}

double CriticalAbundanceReceiver::getCriticalNumberDensity(){
    return criticalNumberDensity_;
}

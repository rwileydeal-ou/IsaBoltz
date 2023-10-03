#include <cmd/DeltaNeff/EstimateReceiver.h>

EstimateDeltaNeffReceiver::EstimateDeltaNeffReceiver( Connection& connection, const Models::ParticleEvolution& particleEvo, const Models::Particle& parent, std::vector< std::vector< boost::uuids::uuid > > childrenIdPairs )
    : DeltaNeffReceiver( connection, particleEvo )
{
    parent_ = parent;
    childrenIdPairs_ = childrenIdPairs;
    deltaNeff_.ParentId = parent_.Id;
    deltaNeff_.InputId = connection_.InputId;
    deltaNeff_.ChildrenIds = childrenIdPairs_;
}

EstimateDeltaNeffReceiver::~EstimateDeltaNeffReceiver(){
}

// TODO: this is a placeholder for now, need to revise once fields other than modulus are implemented!
double EstimateDeltaNeffReceiver::calculateRhoTD(double TDecay, double gStrEnt_TD){
    if (parent_.Key != "modulus"){
        connection_.Log.Warn("USING OUTDATED METHOD FOR RHO(TD)! NEED TO UPDATE!");
        throw NotImplementedException();
    } 

    double mass = parent_.Mass;
    auto Tosc = pullTempOsc();
    double amplitude = parent_.CohOscComponents.Amplitude;
    double gStrEnt_Tosc = GStar::CalculateEntropic( connection_, Tosc.Temperature );

    double rhoTD = 0.5 * std::pow( mass, 2. ) * std::pow( amplitude, 2. ) * std::pow( TDecay / Tosc.Temperature, 3. ) * gStrEnt_TD / gStrEnt_Tosc;
    return rhoTD;
}

Models::TempDecay EstimateDeltaNeffReceiver::pullTempDecay(){
    Models::TempDecay TD;

    DbManager db(connection_);
    db.Open();

    auto statement = Statements::TempDecay(TD, Statements::StatementType::Read);
    auto filter = Filters::TempDecay(connection_.InputId, parent_.Id);
    statement.AddFilter( filter );
    auto cb = Callbacks::TempDecay();

    db.Execute( statement, cb.Callback, cb.CallbackReturn );
    db.Close();

    if ( cb.CallbackReturn.TempDecays.size() != 1 ){
        throw std::logic_error("Could not find unique TempDecays");
    }

    TD = cb.CallbackReturn.TempDecays.front();
    return TD;
}

Models::TempOscillation EstimateDeltaNeffReceiver::pullTempOsc(){
    Models::TempOscillation Tosc;

    DbManager db(connection_);
    db.Open();

    auto statement = Statements::TempOsc(Tosc, Statements::StatementType::Read);
    auto filter = Filters::TempOsc(connection_.InputId, parent_.Id);
    statement.AddFilter( filter );
    auto cb = Callbacks::TempOsc();

    db.Execute( statement, cb.Callback, cb.CallbackReturn );
    db.Close();

    if ( cb.CallbackReturn.TempOscs.size() != 1 ){
        throw std::logic_error("Could not find unique TempOscillation");
    }

    Tosc = cb.CallbackReturn.TempOscs.front();
    return Tosc;
}

Models::TempEquality EstimateDeltaNeffReceiver::pullTempEquality(){
    Models::TempEquality TE;

    DbManager db(connection_);
    db.Open();

    auto statement = Statements::TempEquality(TE, Statements::StatementType::Read);
    auto filter = Filters::TempEquality(connection_.InputId, parent_.Id);
    statement.AddFilter( filter );
    auto cb = Callbacks::TempEquality();

    db.Execute( statement, cb.Callback, cb.CallbackReturn );
    db.Close();

    if ( cb.CallbackReturn.TempEqualities.size() != 1 ){
        throw std::logic_error("Could not find unique TempEquality");
    }

    TE = cb.CallbackReturn.TempEqualities.front();
    return TE;
}

std::vector< Models::PartialWidth > EstimateDeltaNeffReceiver::pullPartialWidths(){
    std::vector< Models::PartialWidth > children;
    Models::PartialWidth child;

    DbManager db(connection_); 
    db.Open();

    for (auto& childIdPair : childrenIdPairs_){
        auto statement = Statements::PartialWidth(child, Statements::StatementType::Read);
        auto filter = Filters::PartialWidth(connection_.InputId, parent_.Id, childIdPair);
        statement.AddFilter( filter );
        auto cb = Callbacks::PartialWidth();
        db.Execute( statement, cb.Callback, cb.CallbackReturn );

        if (cb.CallbackReturn.PartialWidths.size() > 0){
            children.push_back( cb.CallbackReturn.PartialWidths.front() );
        }
    }

    db.Close();

    if (childrenIdPairs_.size() != children.size()){
        throw std::logic_error("Error pulling partial widths");
    }

    return children;
}

Models::TotalWidth EstimateDeltaNeffReceiver::pullTotalWidth(){
    Models::TotalWidth totalWidth;

    DbManager db(connection_);
    db.Open();

    auto statement = Statements::TotalWidth(totalWidth, Statements::StatementType::Read);
    auto filter = Filters::TotalWidth(connection_.InputId, parent_.Id);
    statement.AddFilter( filter );
    auto cb = Callbacks::TotalWidth();

    db.Execute( statement, cb.Callback, cb.CallbackReturn );
    db.Close();

    if ( cb.CallbackReturn.TotalWidths.size() != 1 ){
        throw std::logic_error("Could not find unique TotalWidth");
    }

    totalWidth = cb.CallbackReturn.TotalWidths.front();
    return totalWidth;
}

// This method calculates an estimate for Delta N_eff
// See e.g. arXiv 1301.7428 Eq. 5.1 
// This does NOT factor in cascade decays
// e.g. if saxions are produced from parent, does not factor in their decays which add entropy dilution and additional axions
double EstimateDeltaNeffReceiver::calculateDeltaNeff(){
    auto TD = pullTempDecay();
    auto TE = pullTempEquality();

    // TODO: We should make g_*S(T) at T~eV configurable! 
    // For now, just freezing at g_*S(~eV) = 3.9
    double gStrEnt_T = 3.9;
    double gStrEnt_TD = GStar::CalculateEntropic(connection_, TD.Temperature);

    // To make the entropy dilution factor precise, there should be a g_*(T_S) / g_*(T_D) factor
    // where T_S is the temperature where entropy injection becomes dominant
    // i.e. H(T)_matter = H(T)_decay
    double TS = std::pow( TE.Temperature * pow( TD.Temperature, 4. ), 1./5. );
    double preEnt = GStar::Calculate( connection_, TS ) / TD.GStar;

    // If entropy dilution factor should not go below 1 - no enhancement, only dilution!
    double entropyDilution = std::max( 1., TE.Temperature / TD.Temperature );

    double totalBR = 0.;
    auto partialWidths = pullPartialWidths();
    auto totalWidth = pullTotalWidth();
    for (auto& partialWidth : partialWidths){
        totalBR += ( partialWidth.Width / totalWidth.Width );
    }

    double prefactor = 120. / 7. / std::pow( M_PI, 2. ) * std::pow( 11./4., 4./3. );
    double midFactor = std::pow( gStrEnt_T / gStrEnt_TD, 4./3. ) / std::pow( TD.Temperature, 4. ) / entropyDilution * totalBR;
    // TODO: this must be updated once other fields are supported
    double rhoTD = calculateRhoTD(TD.Temperature, gStrEnt_TD);

    return prefactor * midFactor * rhoTD;
}


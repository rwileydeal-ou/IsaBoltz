#include <cmd/SolveBoltzmann/StepBuilder/CascadeBranchingRatios.h>

using namespace std;

CascadeBranchingRatios::CascadeBranchingRatios(
    Connection& connection, 
    DbManager& db,
    const BoltzmannData& data,
    std::unordered_map< boost::uuids::uuid, Models::Particle, boost::hash<boost::uuids::uuid> >& particleCache
) : 
    connection_(connection),
    data_(data),
    db_(db),
    particleCache_(particleCache)
{
}
CascadeBranchingRatios::~CascadeBranchingRatios(){
}

const Models::Particle& CascadeBranchingRatios::pullParticle(
    const boost::uuids::uuid& particleId
) {
    // ---- 1. Check in-memory cache ----
    auto it = particleCache_.find(particleId);
    if (it != particleCache_.end()) {
        return it->second;
    }

    // ---- 2. Search in already-loaded particles ----
    auto r = std::find_if(
        data_.Particles.begin(), data_.Particles.end(),
        [&particleId](const Models::Particle& part) {
            return part.Id == particleId;
        });

    if (r != data_.Particles.end()) {
        const auto& inserted = particleCache_.emplace(particleId, *r).first->second;
        return inserted;
    }

    // ---- 3. Fallback to DB (should happen only once per unknown particle) ----
    Models::Particle p;
    auto statement = Statements::Particle(p, Statements::StatementType::Read);
    auto filter = Filters::Particle(particleId, Filters::WhereUUID::Id);
    statement.AddFilter(filter);
    auto cb = Callbacks::Particle();
    db_.Execute(statement, cb.Callback, cb.CallbackReturn);

    if (cb.CallbackReturn.Particles.size() != 1)
        throw std::logic_error("Could not pull unique particle");

    const auto& loaded = cb.CallbackReturn.Particles.front();
    const auto& inserted = particleCache_.emplace(particleId, loaded).first->second;
    return inserted;
}

// this helper function computes R-odd to the LSP, assuming 2-body cascade decay to R-odd+R-even ends in 1 LSP
// excludes species tracked separately
double CascadeBranchingRatios::Br_Rodd_to_LSP(
    const ParticleData& parent, 
    const ParticleData& daughter 
){
    double br = 0.;
    const auto& widths = data_.PartialWidths.at(parent.ParticleKey);
    for ( auto& width : widths ){
        // if not 2-body decay, continue 
        // TODO: handle higher body decays
        if ( width.Width == 0. || width.ChildrenIds.size() != 2 ){
            continue;
        }
        auto cID1 = width.ChildrenIds[0];
        auto child1 = pullParticle( cID1 );
        auto cID2 = width.ChildrenIds[1];
        auto child2 = pullParticle( cID2 );

        // check to make sure exactly one child is R-odd
        if ( 
            ( (int)child1.Charges[0].Value == -1 && (int)child2.Charges[0].Value == -1)
            || ( (int)child1.Charges[0].Value == 1 && (int)child2.Charges[0].Value == 1)
        ){
            continue;
        }            

        // don't include if 1 R-odd final state and IS tracked separately
        bool widthTrackedSeparately = false;
        for( auto& p : data_.ParticleDatas ){
            // only look at valid tracked R-odd decays...
            if ( p.ProductionMechanism != ParticleProductionMechanism::THERMAL || p.Charges[0].Value != -1 ){
                continue;
            }
            // TODO: FINAL CHECK HERE IS A BUG WORKAROUND WHERE OTHER PARTICLE HAS DECAYED AND IS TURNED OFF BEFORE PARENT HAS BEGUN TO DECAY
            // TREAT AS RADIATION IF HAS ALREADY DECAYED TO AT LEAST GET ENTROPY PRODUCTION IN CASCADE DECAYS
            // PROPER SOLUTION IS TO REWRITE EQUATIONS SO NON-SINGULAR WHEN SPECIES IS ENTIRELY DECAYED

            // make sure we don't accidentally discard direct parent->LSP decay
            if ( p.ParticleId != daughter.ParticleId 
                && (p.ParticleId == child1.Id || p.ParticleId == child2.Id)
//                && p.IsActive
            ){
                widthTrackedSeparately = true;
            }
        }
        if (!widthTrackedSeparately){
            br += width.Width / parent.TotalWidth;
        }
    }
    return br;
}

// this helper function computes R-odd to R-odd where the daughter is NOT the LSP and is tracked separately
double CascadeBranchingRatios::Br_Rodd_to_Rodd(
    const ParticleData& parent, 
    const ParticleData& daughter
){
    double br = 0.;
    const auto& widths = data_.PartialWidths.at(parent.ParticleKey);
    for ( auto& width : widths ){
        // if not 2-body, or if NEITHER width ID corresponds to the daughter, keep going
        if ( width.Width == 0. || width.ChildrenIds.size() != 2 || ( width.ChildrenIds[0] != daughter.ParticleId && width.ChildrenIds[1] != daughter.ParticleId ) ){
            continue;
        }
        auto cID1 = width.ChildrenIds[0];
        auto child1 = pullParticle( cID1 );
        auto cID2 = width.ChildrenIds[1];
        auto child2 = pullParticle( cID2 );

        // check to make sure one child is R-odd
        if ( 
            ( (int)child1.Charges[0].Value == -1 && (int)child2.Charges[0].Value == -1)
            || ( (int)child1.Charges[0].Value == 1 && (int)child2.Charges[0].Value == 1)
        ){
            connection_.Log.Warn("Detected R-odd -> R-odd+R-odd/R-even+R-even width"); // this shouldn't ever happen, but should check to make sure
            continue;
        }            

        // take only the widths associated to child
        if ( child1.Key == daughter.ParticleKey || child2.Key == daughter.ParticleKey ){
            br += width.Width / parent.TotalWidth;
        }
    }
    return br;
}

double CascadeBranchingRatios::Br_Rodd_to_Reven(
    const ParticleData& parent, 
    const ParticleData& daughter
){
    double br = 0.;
    const auto& widths = data_.PartialWidths.at(parent.ParticleKey);
    for ( auto& width : widths ){
        // if not 2-body, or if NEITHER width ID corresponds to the daughter, keep going
        if ( width.Width == 0. || width.ChildrenIds.size() != 2 || ( width.ChildrenIds[0] != daughter.ParticleId && width.ChildrenIds[1] != daughter.ParticleId ) ){
            continue;
        }
        auto cID1 = width.ChildrenIds[0];
        auto child1 = pullParticle( cID1 );
        auto cID2 = width.ChildrenIds[1];
        auto child2 = pullParticle( cID2 );

        // check to make sure one child is R-odd/one is R-even
        if ( 
            ( (int)child1.Charges[0].Value == -1 && (int)child2.Charges[0].Value == -1)
//            || ( (int)child1.Charges[0].Value == 1 && (int)child2.Charges[0].Value == 1)  // NOTE: we're temporarily removing this to support RPV models
        ){
            connection_.Log.Warn("Detected R-odd -> R-odd+R-odd/R-even+R-even width"); // this shouldn't ever happen, but should check to make sure
            continue;
        }            

        // take only the widths associated to child
        if ( child1.Key == daughter.ParticleKey || child2.Key == daughter.ParticleKey ){
            br += width.Width / parent.TotalWidth;
        }
    }
    return br;
}

// this helper function computes R-odd to radiation, assuming any 2-body cascade decay ends in radiation release
// excludes species where both daughters are explicitly tracked separately
double CascadeBranchingRatios::Br_Rodd_to_radiation(
    const ParticleData& parent, 
    const ParticleData& daughter
){
    double br = 1.;

    // check to make sure we don't include things tracked separately
    const auto& widths = data_.PartialWidths.at(parent.ParticleKey);
    for ( auto& width : widths ){
        // only 2-body decays
        if ( width.Width == 0. || width.ChildrenIds.size() != 2 ){
            continue;
        }
        auto cID1 = width.ChildrenIds[0];
        auto child1 = pullParticle( cID1 );
        auto cID2 = width.ChildrenIds[1];
        auto child2 = pullParticle( cID2 );

        // check to make sure one child is R-odd
        if ( 
            ( (int)child1.Charges[0].Value == -1 && (int)child2.Charges[0].Value == -1)
//            || ( (int)child1.Charges[0].Value == 1 && (int)child2.Charges[0].Value == 1)  // NOTE: temporarily removing this to allow RPV models
        ){
            connection_.Log.Warn("Detected R-odd -> R-odd+R-odd/R-even+R-even width"); // this shouldn't ever happen, but should check to make sure
            continue;
        }            

        bool widthTrackedSeparately = false;
        for ( auto& p : data_.ParticleDatas ){
            // only look at valid R-odd decays
            if ( p.ProductionMechanism != ParticleProductionMechanism::THERMAL ){
                continue;
            }
            // if daughter is tracked separately and not radiation (already filtered), continue trying to match keys
            if ( p.ParticleKey == child1.Key ){
                // we've matched one key as tracked separately, but second key may not necessarily be same as first
                for ( auto& p2 : data_.ParticleDatas ){
                    // TODO: FINAL CHECK HERE IS A BUG WORKAROUND WHERE OTHER PARTICLE HAS DECAYED AND IS TURNED OFF BEFORE PARENT HAS BEGUN TO DECAY
                    // TREAT AS RADIATION IF HAS ALREADY DECAYED TO AT LEAST GET ENTROPY PRODUCTION IN CASCADE DECAYS
                    // PROPER SOLUTION IS TO REWRITE EQUATIONS SO NON-SINGULAR WHEN SPECIES IS ENTIRELY DECAYED
                    if ( p2.ParticleKey == child2.Key && p.ProductionMechanism != ParticleProductionMechanism::RADIATION 
//                        && p2.IsActive 
                    ){
                        // now we've verified both children in width are tracked separately and neither is radiation
                        widthTrackedSeparately = true;
                        break;
                    }
                }
            }
        }
        if ( widthTrackedSeparately ){
            br -= width.Width / parent.TotalWidth;
        }
    }
    return br;
}

double CascadeBranchingRatios::Br_Reven_to_Reven(
    const ParticleData& parent, 
    const ParticleData& daughter
){
    double br = 0.;

    const auto& widths = data_.PartialWidths.at(parent.ParticleKey);
    for ( auto& width : widths ){
        if ( width.Width == 0. || width.ChildrenIds.size() != 2 ){
            continue;
        }
        auto cID1 = width.ChildrenIds[0];
        auto child1 = pullParticle( cID1 );
        auto cID2 = width.ChildrenIds[1];
        auto child2 = pullParticle( cID2 );

        // check to make sure both children are R-even
        if ( (int)child1.Charges[0].Value != 1 || (int)child2.Charges[0].Value != 1 ){
            continue;
        }

        // if child is R-even and NOT radiation, take only the widths associated to child
        if ( child1.Key == daughter.ParticleKey || child2.Key == daughter.ParticleKey ){
            br += width.Width / parent.TotalWidth;
        }
    }
    return br;
}

double CascadeBranchingRatios::Br_Reven_to_radiation(
    const ParticleData& parent, 
    const ParticleData& daughter
){
    double br = 1.;

    // go through all partial widths of the parent
    // we assume that every cascade decay rapidly thermalizes so total decay can be treated as radiation
    // if a width matches a tracked key, don't include it since tracked separately
    // i.e. BR = 1 - species tracked separately
    // TODO: refactor this and make more efficient...
    const auto& widths = data_.PartialWidths.at(parent.ParticleKey);
    for ( auto& width : widths ){
        if ( width.Width == 0. || width.ChildrenIds.size() != 2 ){
            continue;
        }
        auto cID1 = width.ChildrenIds[0];
        auto child1 = pullParticle( cID1 );
        auto cID2 = width.ChildrenIds[1];
        auto child2 = pullParticle( cID2 );

        bool widthTrackedSeparately = false;
        for ( auto& p : data_.ParticleDatas ){
            if ( p.ProductionMechanism != ParticleProductionMechanism::THERMAL ){
                continue;
            }
            if ( p.ParticleKey == child1.Key ){
                for ( auto& p2 : data_.ParticleDatas ){
                    // TODO: FINAL CHECK HERE IS A BUG WORKAROUND WHERE OTHER PARTICLE HAS DECAYED AND IS TURNED OFF BEFORE PARENT HAS BEGUN TO DECAY
                    // TREAT AS RADIATION IF HAS ALREADY DECAYED TO AT LEAST GET ENTROPY PRODUCTION IN CASCADE DECAYS
                    // PROPER SOLUTION IS TO REWRITE EQUATIONS SO NON-SINGULAR WHEN SPECIES IS ENTIRELY DECAYED
                    if ( p2.ParticleKey == child2.Key && p2.ProductionMechanism == ParticleProductionMechanism::THERMAL 
//                        && p2.IsActive 
                    ){
                        widthTrackedSeparately = true;
                        break;
                    }
                }
            }
        }
        if ( widthTrackedSeparately ){
            br -= width.Width / parent.TotalWidth;
        }
    }
    return br;
}

// this method computes R-even -> LSP + LSP, assuming 2-body cascade decay ends in 1 LSP
// excludes if BOTH children are tracked separately 
double CascadeBranchingRatios::Br_Reven_to_LSP(
    const ParticleData& parent, 
    const ParticleData& daughter
){
    double br = 0.;

    const auto& widths = data_.PartialWidths.at(parent.ParticleKey);
    for ( auto& width : widths ){
        // only continue if 2-body decay
        if ( width.Width == 0. || width.ChildrenIds.size() != 2 ){
            continue;
        }
        auto cID1 = width.ChildrenIds[0];
        auto child1 = pullParticle( cID1 );
        auto cID2 = width.ChildrenIds[1];
        auto child2 = pullParticle( cID2 );

        // TODO: REMOVE THIS - HACK FOR DISS
//        if ( child1.Key.find("higgs") != std::string::npos && child2.Key.find("higgs") != std::string::npos){
//            br += 0.01 * width.Width / parent.TotalWidth;
//        }

        // check to make sure both children are R-odd
        if ( (int)child1.Charges[0].Value == 1 || (int)child2.Charges[0].Value == 1 ){
            continue;
        }

        bool widthTrackedSeparately = false;
        for ( auto& p : data_.ParticleDatas ){
            // only look at valid R-odd decays
            if ( p.ProductionMechanism != ParticleProductionMechanism::THERMAL || p.Charges[0].Value != -1 ){
                continue;
            }
            // if daughter is tracked separately and NOT LSP, continue trying to match keys
            if ( p.ParticleKey == child1.Key && !p.IsLSP ){
                // we've matched one key as tracked separately, but second key may not necessarily be same as first
                for ( auto& p2 : data_.ParticleDatas ){
                    // TODO: FINAL CHECK HERE IS A BUG WORKAROUND WHERE OTHER PARTICLE HAS DECAYED AND IS TURNED OFF BEFORE PARENT HAS BEGUN TO DECAY
                    // TREAT AS RADIATION IF HAS ALREADY DECAYED TO AT LEAST GET ENTROPY PRODUCTION IN CASCADE DECAYS
                    // PROPER SOLUTION IS TO REWRITE EQUATIONS SO NON-SINGULAR WHEN SPECIES IS ENTIRELY DECAYED
                    if ( p2.ParticleKey == child2.Key && !p2.IsLSP 
//                        && p2.IsActive 
                    ){
                        // now we've verified both children in width are tracked separately, and NEITHER is the LSP
                        widthTrackedSeparately = true;
                        break;
                    }
                }
            }
        }
        if ( !widthTrackedSeparately ){
            br += width.Width / parent.TotalWidth;
        }
    }
    return br;
}

double CascadeBranchingRatios::Br_Reven_to_Rodd(
    const ParticleData& parent, 
    const ParticleData& daughter
){
    double br = 0.;

    const auto& widths = data_.PartialWidths.at(parent.ParticleKey);
    for ( auto& width : widths ){
        if ( width.Width == 0. || width.ChildrenIds.size() != 2 ){
            continue;
        }
        auto cID1 = width.ChildrenIds[0];
        auto child1 = pullParticle( cID1 );
        auto cID2 = width.ChildrenIds[1];
        auto child2 = pullParticle( cID2 );

        // check to make sure both children are R-odd
        if ( (int)child1.Charges[0].Value == 1 || (int)child2.Charges[0].Value == 1 ){
            continue;
        }

        // if child is R-odd and NOT the LSP, take only the widths associated to child
        if ( child1.Key == daughter.ParticleKey || child2.Key == daughter.ParticleKey ){
            br += width.Width / parent.TotalWidth;
        }
    }
    return br;
}

double CascadeBranchingRatios::Calculate(
    const ParticleData& parent, 
    const ParticleData& daughter
){    
    // if the daughter is kinematically inaccessible, no point in continuing - but can't tell other particle, so only look at parent/daughter masses
    if (
        parent.Mass <= daughter.Mass 
//        || !daughter.IsActive
    ){
        return 0.;
    }
    
    // if parent is R-odd and 2 body decay, one child is R-odd and one is R-even
    // since we're analyzing each child separately in the function calling this method, only look at the final "cascade decay" products
    if ( (int)parent.Charges[0].Value == -1 ){
        if ( (int)daughter.Charges[0].Value == -1 && !daughter.IsLSP ){
            return Br_Rodd_to_Rodd( parent, daughter );
        } else if ( (int)daughter.Charges[0].Value == -1 && daughter.IsLSP ){
            return Br_Rodd_to_LSP( parent, daughter );
        } else if ( (int)daughter.Charges[0].Value == 1 && daughter.ProductionMechanism == ParticleProductionMechanism::RADIATION ){
            // if child is R-even, effectively SM - so can treat as radiation
            return Br_Rodd_to_radiation( parent, daughter );
        } else if ( (int)daughter.Charges[0].Value == 1 && daughter.ProductionMechanism == ParticleProductionMechanism::THERMAL ){
            return Br_Rodd_to_Reven( parent, daughter );
        }
    }

    // if parent is R-even and 2 body decay, both children are either R-odd or R-even
    if ( (int)parent.Charges[0].Value == 1 ){
        if ( (int)daughter.Charges[0].Value == -1 && !daughter.IsLSP ){
            return Br_Reven_to_Rodd( parent, daughter );
        } else if ( (int)daughter.Charges[0].Value == -1 && daughter.IsLSP ){
            return Br_Reven_to_LSP( parent, daughter );
        } else if ( (int)daughter.Charges[0].Value == 1 && daughter.ProductionMechanism == ParticleProductionMechanism::RADIATION ){
            return Br_Reven_to_radiation( parent, daughter );
        } else if ( (int)daughter.Charges[0].Value == 1 && daughter.ProductionMechanism == ParticleProductionMechanism::THERMAL ){
            return Br_Reven_to_Reven( parent, daughter );
        }
    }

    return 0;
}


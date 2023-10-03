#include <PQ/PotentialMinima.h>

using namespace std;

PotentialMinima::PotentialMinima(){}
PotentialMinima::~PotentialMinima(){}

PotentialMinima::dcmplx PotentialMinima::cck_potential_solver(ModelBase model, dcmplx x){
    double f = 1.;
    double Af = 2.5 * model.A0;
    double mX = model.Masses.M0;
    double mY = model.Masses.M0;
    double mP = model.Constants.mPlanck;

    double quadratic = (1./81.) * pow(1. / (f * mP), 2.) * ( 18. * pow(mY, 2.) + 27. * pow(mX, 2.) - 3. * pow(Af, 2.) );
    double linear = (1./81.) * pow(1. / (f * mP), 4.) * pow(mY, 2.) * ( pow(mY, 2.) + 6. * pow(mX, 2.) - pow(Af, 2.) );
    double constant = pow(mX, 2.) * pow(mY, 4.) / (243. * pow(f * mP, 6.));

    return ( pow(x, 3.) + ( quadratic * pow(x, 2.) ) + ( linear * x ) + constant );
}

PecceiQuinnPotential PotentialMinima::cck_potential(ModelBase model, double vX){
    PecceiQuinnPotential pq;
    pq.f = 1.;
    pq.Af = 2.5 * model.A0;
    pq.mX = model.Masses.M0; 
    pq.mY = model.Masses.M0;
    double mP = model.Constants.mPlanck;
    pq.vX = vX;
    pq.vY = - pq.Af * pq.f * pow(pq.vX, 3.) / ( 9. * pow(pq.f, 2.) * pow(pq.vX, 4.) / mP + pow(pq.mY, 2.) * mP );
    pq.vPQ = sqrt( pow(pq.vX, 2.) + 9. * pow(pq.vY, 2.) );
    return pq;
}

// quick and dirty Durand-Kerner method to find roots
// vev is scaled by a:=vx^4/mP^4 to guarantee convergence, then scaled back
void PotentialMinima::Minimize( ModelBase& model, Logger& logger ){
    logger.Info("Minimizing PQ potential");

    dcmplx p(1.e10 / pow(model.Constants.mPlanck, 4.), 0.);
    dcmplx q(1.e12 / pow(model.Constants.mPlanck, 4.), 0.);
    dcmplx r(1.e14 / pow(model.Constants.mPlanck, 4.), 0.);
    dcmplx p0, q0, r0;

    int max_iterations = 200;
    double tol = 1e-9;
    bool done = false;
    int i=0;

    while (i<max_iterations && !done)
    {   
        p0 = p;
        q0 = q;
        r0 = r;

        p = p0 - cck_potential_solver(model, p0)/((p0-q0)*(p0-r0));
        q = q0 - cck_potential_solver(model, q0)/((q0-p)*(q0-r0));
        r = r0 - cck_potential_solver(model, r0)/((r0-p)*(r0-q));

        // if convergence within small epsilon, declare done
        if (abs(p-p0)/abs(p)<tol && abs(q-q0)/abs(q)<tol && abs(r-r0)/abs(r)<tol){
            done = true;
        }
        ++i;
    }

    double root1 = 0., root2 = 0., root3 = 0.;
    if (abs(p) != 0. && p.real()/abs(p) > .99){
        root1 = p.real();
    }
    if (abs(q) != 0. && q.real()/abs(q) > .99){
        root2 = q.real();
    }
    if (abs(r) != 0. && r.real()/abs(r) > .99){
        root3 = r.real();
    }

    double maxRoot = max( {root1, root2, root3} );
    double vX = pow(maxRoot, 1./4.) * model.Constants.mPlanck;
    if (std::isinf(vX) || std::isnan(vX)){
        logger.Error("Error finding root!");
        throw logic_error("");
    }
    if (vX == 0.){
        model.IsajetError = -2;
        throw logic_error("No real root exists!");
    }

    auto potential = cck_potential(model, vX);
    model.PQSector.Potential.vX = potential.vX;
    model.PQSector.Potential.vY = potential.vY;
    model.PQSector.Potential.vPQ = potential.vPQ;
    model.PQSector.Potential.Af = potential.Af;
    model.PQSector.Potential.f = potential.f;
    // TODO: remove later
    model.PQSector.Potential.mX = potential.mX;
    model.PQSector.Potential.mY = potential.mY;
    
    model.PQSector.Fa = sqrt(2.) * model.PQSector.Potential.vPQ;
    double mu = model.PQSector.Potential.g * pow(model.PQSector.Potential.vX, 2.) / model.Constants.mPlanck;
    logger.Info("Updating mu: " + to_string(mu));
    model.Options.NonUniversalSugra.WeakInput.weakMu = mu;
}

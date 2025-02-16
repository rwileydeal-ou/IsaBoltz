#include <ModelIndependent/Densities/NumberDensityEq.h>

NumberDensityEq::NumberDensityEq(/* args */){}
NumberDensityEq::~NumberDensityEq(){}

double NumberDensityEq::Calculate(double mass, double T, double degF, Models::ParticleStatistics stats){
    double X = T / mass;
    double neq = 0;
    double neqNonRelativistic = pow(mass, 3.) * pow( X / (2. * M_PI), 3./2.) * exp(-1. / X) * ( 1. + (15. / 8.) * X + (105. / 128.) * pow(X, 2.));
    double neqIntermediate = pow(mass, 3.) * X * boost::math::cyl_bessel_k(2., 1./X) / (2. * pow(M_PI, 2.));
    double neqRelativistic = 0.;
      switch (stats)
      {
         case Models::ParticleStatistics::Boson:
            neqRelativistic = boost::math::zeta(3.) * pow(T, 3.) / pow(M_PI, 2.);
            break;
         case Models::ParticleStatistics::Fermion:
            neqRelativistic = (3./4.) * boost::math::zeta(3.) * pow(T, 3.) / pow(M_PI, 2.);
            break;            
         default:
            throw_with_trace( std::logic_error("Select valid particle statistics!") );
      }

   if (X < 0.1){
      neq = neqNonRelativistic;
   }
   else if (X >= 0.1 && X <= 0.2){
      // interpolate further...
      neq = neqNonRelativistic + ( neqIntermediate - neqNonRelativistic ) / 0.1 * ( X - 0.1 );
   }
   else if (X > 0.2 && X < 1.){
      // intermediate regime
      neq = neqIntermediate;
   }
   else if (X >= 1. && X <= 1.5){
      // interpolate further...
      neq = neqIntermediate + ( neqRelativistic - neqIntermediate ) / 0.5 * ( X - 1. );
   }
   else{
      neq = neqRelativistic;
   }
   // TODO: do this better (damn legacy code)
   neq = degF * neq;
    return neq;

    /* TODO: check this out and fix
!....Numerical fit (NOT WORKING ????)
   if (DEGF(I).GT.0d0) then                    ! Ultra-relativistic
      NUR = ZETA3*T**3/PI**2
   else
      NUR = (3d0/4d0)*ZETA3*T**3/PI**2
   end if

   if (MASS(I).EQ.0d0) then
      NEQ = NUR
   else      
      if (T.GT.MASS(I)/100d0) then ! Non-relativistic
         K2=DBESK0(MASS(I)/T) + 2d0*DBESK1(MASS(I)/T)*T/MASS(I)
         NNR=MASS(I)**2*T*K2/(2d0*PI**2) 
      else
         NNR=((MASS(I)*T/(2d0*PI))**(3d0/2d0))*DEXP(-MASS(I)/T)
      end if
      do J=1,30
         A(J) = 0d0
      end do
      if (DEGF(I).GT.0d0) then
         A(1) = 0.000361225d0
         A(2) = -0.000215089d0
         A(3) = -0.0118175d0
         A(4) = 0.0363855d0
         A(5) = -0.0310704d0
         A(6) = 0.0133859d0
         A(7) = -0.0039848d0
         A(8) = 0.000848539d0
         A(9) = -0.000130162d0
         A(10) = 0.0000142652d0
         A(11) = -1.08932d-6
         A(12) = 5.50418d-8
         A(13) = -1.65361d-9
         A(14) = 2.2358d-11
      else
         A(1) = -0.000409955d0
         A(2) = 0.00132189d0
         A(3) = 0.00602352d0
         A(4) = -0.0214955d0
         A(5) = 0.00765591d0
         A(6) = -0.00365523d0
         A(7) = 0.00122814d0
         A(8) = -0.000287047d0
         A(9) = 0.0000472529d0
         A(10) = -5.47044d-6
         A(11) = 4.36382d-7
         A(12) = -2.28493d-8
         A(13) = 7.07099d-10
         A(14) = -9.80306d-12
      end if

      NFIT=DEXP(-MASS(I)/T)*MASS(I)**3*(T/(2d0*PI))**(3d0/2d0)&
      *(1d0 + (15d0/8d0)*(T/MASS(I)) + (105d0/128d0)*(T/MASS(I))**2)

      do J=1,30
         NFIT = NFIT + A(J)*T**J
      end do

      if (T.GT.10d0*MASS(I)) then
         NEQ = NUR
      else if (T.GT.MASS(I)/2d0) then
         NEQ = NFIT
      else if (T.GT.MASS(I)/3d0) then
         if (DEGF(I).GT.0d0) then
            NEQ = MAX(NFIT,NNR)
         else
            NEQ = MIN(NFIT,NNR)
         end if
      else
         NEQ = NNR
      end if
   end if
    */
}

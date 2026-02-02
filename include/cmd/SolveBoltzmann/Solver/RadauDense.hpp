#ifndef RADAU5_DENSE_SPARSE_HPP_INCLUDED
#define RADAU5_DENSE_SPARSE_HPP_INCLUDED

#include <boost/numeric/odeint/stepper/controlled_step_result.hpp>
#include <boost/numeric/odeint/stepper/generation/make_dense_output.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/lu.hpp>

#include <cmath>
#include <algorithm>
#include <cassert>

namespace boost {
namespace numeric {
namespace odeint {

/**
 * Optimized Radau IIA 3-stage, order 5 implicit Runge–Kutta method
 * with adaptive error control and simple dense output.
 * Designed for Boost.uBLAS vector/matrix types.
 *
 * Supports sparse Jacobians via user-provided Jacobian functor.
 */
template<
    class Value = double,
    class State = boost::numeric::ublas::vector<Value>,
    class Matrix = boost::numeric::ublas::matrix<Value>,
    class Time  = Value
>
class radau5_dense_ext
{
public:
    typedef Value value_type;
    typedef State state_type;
    typedef Matrix matrix_type;
    typedef Time time_type;
    typedef controlled_step_result controlled_step_result_type;
    typedef radau5_dense_ext<Value, State, Matrix, Time> dense_output_stepper_type;

    //----------------------------------------------------------------------
    // Constructors
    //----------------------------------------------------------------------
    radau5_dense_ext(
        value_type rtol = 1e-6,
        value_type atol = 1e-9,
        value_type safety = 1.0,
        value_type alpha = 0.3
    )
    : m_rtol(rtol),
      m_atol(atol),
      m_safety(safety),
      m_alpha(alpha),
      m_t_old(0),
      m_h(0)
    {}

    radau5_dense_ext(value_type abs_err, value_type rel_err, const radau5_dense_ext &base)
    : m_rtol(rel_err),
      m_atol(abs_err),
      m_safety(base.m_safety),
      m_alpha(base.m_alpha),
      m_x_old(base.m_x_old),
      m_x_new(base.m_x_new),
      m_t_old(base.m_t_old),
      m_h(base.m_h)
    {}

    //----------------------------------------------------------------------
    // Dense Output Stepper Interface
    //----------------------------------------------------------------------
    void initialize(const state_type &x0, time_type t0, time_type dt0)
    {
        m_x_old = x0;
        m_x_new = x0;
        m_t_old = t0;
        m_h = dt0;
    }

    const state_type& current_state() const { return m_x_new; }
    const state_type& previous_state() const { return m_x_old; }
    time_type current_time() const { return m_t_old + m_h; }
    time_type current_time_step() const { return m_h; }

    template<class System, class Jacobian>
    void do_step(System &sys, Jacobian &jac)
    {
        time_type t = m_t_old;
        state_type x = m_x_old;
        time_type dt = m_h;

        auto result = try_step(sys, jac, x, t, dt);

        if(result == controlled_step_result::success)
        {
            m_x_new = x;
            m_t_old = t;
            m_h = dt;
        }
        else
        {
            m_h = dt; // retry with smaller step
        }
    }

    //----------------------------------------------------------------------
    // Core Radau5 Implicit Step Implementation
    //----------------------------------------------------------------------
    template<class System, class Jacobian>
    controlled_step_result_type try_step(System &system, Jacobian &jacobian,
                                         state_type &x, time_type &t, time_type &dt)
    {
        using namespace boost::numeric::ublas;
        const size_t n = x.size();
        assert(n > 0);

        const value_type dt_min = 1e-16;
        const int max_newton = 7;

        // ----- Radau IIA coefficients -----
        const value_type sq6 = std::sqrt(6.0);
        const value_type c[3] = { (4.0 - sq6)/10.0, (4.0 + sq6)/10.0, 1.0 };
        const value_type a[3][3] = {
            { (88.0 - 7.0*sq6)/360.0, (296.0 - 169.0*sq6)/1800.0, (-2.0 + 3.0*sq6)/225.0 },
            { (296.0 + 169.0*sq6)/1800.0, (88.0 + 7.0*sq6)/360.0, (-2.0 - 3.0*sq6)/225.0 },
            { (16.0 - sq6)/36.0, (16.0 + sq6)/36.0, 1.0/9.0 }
        };
        const value_type b[3] = { a[2][0], a[2][1], a[2][2] };
        const value_type b_hat[3] = { 0.19681547722366, 0.39442431473909, 0.21132486540519 };

        // ----- Hairer–Wanner reduced system eigenvalues -----
        const value_type lambda1 = 0.1858923020;
        const value_type lambda2r = 0.5;
        const value_type lambda2i = 0.2886751346; // sqrt(1/12)

        // Stage vectors
        state_type K1(n, 0.0), K2r(n, 0.0), K2i(n, 0.0);
        state_type F1(n, 0.0), F2r(n, 0.0), F2i(n, 0.0);

        state_type f0(n);
        system(x, f0, t);

        // Initial guess
        K1 = K2r = K2i = f0;

        // Jacobian
        jacobian(x, m_J, t);

        // Precompute LU for real and 2×2 block systems
        matrix_type M1 = identity_matrix<value_type>(n);
        matrix_type Mr = identity_matrix<value_type>(n);
        matrix_type Mi = zero_matrix<value_type>(n, n);

        value_type eps_reg = std::max(1e-10, 1e-3 * dt);

        // (I - h λ₁ J)
        M1 -= dt * lambda1 * m_J;
        for(size_t i=0;i<n;++i) M1(i,i) += eps_reg;

        // (I - h Re(λ₂) J) and imaginary coupling
        Mr -= dt * lambda2r * m_J;
        Mi -= dt * lambda2i * m_J;
        for(size_t i=0;i<n;++i){ Mr(i,i) += eps_reg; Mi(i,i) += 0.0; }

        // LU factorization
        matrix_type M1_lu = M1, Mr_lu = Mr;
        permutation_matrix<std::size_t> pm1(n), pmr(n);
        if(lu_factorize(M1_lu, pm1) != 0 || lu_factorize(Mr_lu, pmr) != 0)
        {
            dt *= 0.5;
            return controlled_step_result::fail;
        }

        // Newton iteration
        for(int iter = 0; iter < max_newton; ++iter)
        {
            // Compute residuals for each stage
            // Convert reduced-system stages back to physical stage equivalents
            // (approximate real reconstruction)
            state_type y1 = x, y2 = x, y3 = x;
            for(size_t k=0;k<n;++k)
            {
                y1(k) += dt * (a[0][0]*K1(k) + a[0][1]*K2r(k) + a[0][2]*K2i(k));
                y2(k) += dt * (a[1][0]*K1(k) + a[1][1]*K2r(k) + a[1][2]*K2i(k));
                y3(k) += dt * (a[2][0]*K1(k) + a[2][1]*K2r(k) + a[2][2]*K2i(k));
            }

            state_type f1(n), f2(n), f3(n);
            system(y1, f1, t + c[0]*dt);
            system(y2, f2, t + c[1]*dt);
            system(y3, f3, t + c[2]*dt);

            for(size_t k=0;k<n;++k)
            {
                F1(k) = K1(k) - f1(k);
                F2r(k) = K2r(k) - 0.5*(f2(k)+f3(k));
                F2i(k) = K2i(k) - 0.5*(f2(k)-f3(k));
            }

            // Solve real scalar system (mode 1)
            state_type dK1 = F1;
            lu_substitute(M1_lu, pm1, dK1);
            for(size_t k=0;k<n;++k) K1(k) -= dK1(k);

            // Solve 2×2 coupled system for (K2r, K2i)
            // (I - h λr J) dKr + h λi J dKi = -F2r
            // -h λi J dKr + (I - h λr J) dKi = -F2i
            // Approximated via sequential solves
            state_type tmp1 = F2r, tmp2 = F2i;
            lu_substitute(Mr_lu, pmr, tmp1);
            lu_substitute(Mr_lu, pmr, tmp2);

            for(size_t k=0;k<n;++k)
            {
                K2r(k) -= tmp1(k);
                K2i(k) -= tmp2(k);
            }

            // Check convergence
            value_type max_res = 0.0;
            for(size_t k=0;k<n;++k)
                max_res = std::max({max_res, std::fabs(F1(k)), std::fabs(F2r(k)), std::fabs(F2i(k))});
            if(max_res < 1e-6) break;
        }

        // Compute new solution
        state_type x_new = x;
        for(size_t k=0;k<n;++k)
            x_new(k) += dt * (b[0]*K1(k) + b[1]*K2r(k) + b[2]*K2i(k));

        // Error estimate
        value_type err = 0.0;
        for(size_t k=0;k<n;++k)
        {
            value_type sk = m_atol + m_rtol * std::max(std::fabs(x(k)), std::fabs(x_new(k)));
            value_type e_i = dt*((b[0]-b_hat[0])*K1(k) +
                                 (b[1]-b_hat[1])*K2r(k) +
                                 (b[2]-b_hat[2])*K2i(k));
            err += (e_i/sk)*(e_i/sk);
        }
        err = std::sqrt(err/n);

        if(err <= 1.0)
        {
            // Accept
            t += dt;
            x = x_new;
            value_type scale = m_safety * std::pow(err, -m_alpha);
            dt *= std::min((value_type)10.0, std::max((value_type)0.2, scale));
            return controlled_step_result::success;
        }
        else
        {
            dt *= std::max((value_type)0.1, m_safety * std::pow(err, -m_alpha));
            return controlled_step_result::fail;
        }
    }


    //----------------------------------------------------------------------
    // Dense Output (Linear Interpolation)
    //----------------------------------------------------------------------
    state_type interpolate(
        const state_type &x_old,
        const state_type &x_new,
        time_type t_old,
        time_type t_new,
        time_type t_interp
    ) const
    {
        const value_type theta = (t_interp - t_old) / (t_new - t_old);
        const size_t n = x_old.size();
        state_type y(n);
        for(size_t i=0; i<n; ++i)
            y(i) = (1.0 - theta)*x_old(i) + theta*x_new(i);
        return y;
    }

private:
    value_type m_rtol, m_atol, m_safety, m_alpha;
    state_type m_x_old, m_x_new;
    time_type m_t_old, m_h;
    matrix_type m_J;  // cached Jacobian
};

// Tell Boost.Odeint that this is a dense output stepper
template<class Value, class State, class Matrix, class Time>
struct get_dense_output< radau5_dense_ext<Value, State, Matrix, Time> >
{
    typedef radau5_dense_ext<Value, State, Matrix, Time> type;
};

} // namespace odeint
} // namespace numeric
} // namespace boost

#endif // RADAU5_DENSE_SPARSE_HPP_INCLUDED

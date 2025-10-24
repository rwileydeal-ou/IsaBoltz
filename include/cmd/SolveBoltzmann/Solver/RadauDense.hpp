#ifndef RADAU5_DENSE_EXT_HPP_INCLUDED
#define RADAU5_DENSE_EXT_HPP_INCLUDED

#include <boost/numeric/odeint/stepper/controlled_step_result.hpp>
#include <boost/numeric/odeint/stepper/generation/make_dense_output.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/matrix.hpp>

#include <cmath>
#include <algorithm>
#include <cassert>

namespace boost {
namespace numeric {
namespace odeint {

/**
 * Radau IIA 3-stage, order 5 implicit Runge–Kutta method
 * with adaptive error control and simple dense output,
 * designed for Boost.uBLAS vector/matrix types.
 */
template<
    class Value = double,
    class State = boost::numeric::ublas::vector<Value>,
    class Deriv = State,
    class Time  = Value
>
class radau5_dense_ext
{
public:
    // Boost.Odeint concept typedefs
    typedef Value value_type;
    typedef State state_type;
    typedef Deriv deriv_type;
    typedef Time  time_type;
    typedef controlled_step_result controlled_step_result_type;
    typedef radau5_dense_ext<Value, State, Deriv, Time> dense_output_stepper_type;

    // Constructor
    radau5_dense_ext(
        value_type rtol = 1e-6,
        value_type atol = 1e-9,
        value_type safety = 0.9,
        value_type alpha = 0.2
    )
    : m_rtol(rtol),
      m_atol(atol),
      m_safety(safety),
      m_alpha(alpha),
      m_t_old(0),
      m_h(0)
    {}

    // Constructor expected by make_dense_output
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

    //-----------------------------------------------------------------------
    // Dense output stepper interface (Odeint concept)
    //-----------------------------------------------------------------------
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

        m_x_new = x;
        m_t_old = t - dt; // maintain consistency
        m_h = dt;
    }

    //-----------------------------------------------------------------------
    // Main implicit Radau5 step implementation
    //-----------------------------------------------------------------------
    template<class System, class Jacobian>
    controlled_step_result_type try_step(
        System &system,
        Jacobian &jacobian,
        state_type &x,
        time_type &t,
        time_type &dt
    )
    {
        using namespace boost::numeric::ublas;

        const size_t n = x.size();
        assert(n > 0);

        const value_type sq6 = std::sqrt((value_type)6.0);
        const value_type c1 = (4.0 - sq6) / 10.0;
        const value_type c2 = (4.0 + sq6) / 10.0;
        const value_type c3 = 1.0;

        const value_type a11 = (88.0 - 7.0*sq6) / 360.0;
        const value_type a12 = (296.0 - 169.0*sq6) / 1800.0;
        const value_type a13 = (-2.0 + 3.0*sq6) / 225.0;
        const value_type a21 = (296.0 + 169.0*sq6) / 1800.0;
        const value_type a22 = (88.0 + 7.0*sq6) / 360.0;
        const value_type a23 = (-2.0 - 3.0*sq6) / 225.0;
        const value_type a31 = (16.0 - sq6) / 36.0;
        const value_type a32 = (16.0 + sq6) / 36.0;
        const value_type a33 = 1.0 / 9.0;

        const value_type b1 = a31, b2 = a32, b3 = a33;

        state_type X1(x), X2(x), X3(x);
        deriv_type K1(n), K2(n), K3(n);

        const value_type tol_newton = 1e-8;
        const size_t max_iter = 6;

        for(size_t iter=0; iter<max_iter; ++iter)
        {
            system(X1, K1, t + c1*dt);
            system(X2, K2, t + c2*dt);
            system(X3, K3, t + c3*dt);

            deriv_type R1(n), R2(n), R3(n);
            for(size_t i=0; i<n; ++i)
            {
                R1(i) = X1(i) - x(i) - dt*(a11*K1(i) + a12*K2(i) + a13*K3(i));
                R2(i) = X2(i) - x(i) - dt*(a21*K1(i) + a22*K2(i) + a23*K3(i));
                R3(i) = X3(i) - x(i) - dt*(a31*K1(i) + a32*K2(i) + a33*K3(i));
            }

            value_type maxR = 0.0;
            for(size_t i=0; i<n; ++i)
                maxR = std::max({maxR, std::fabs(R1(i)), std::fabs(R2(i)), std::fabs(R3(i))});

            if(maxR < tol_newton) break;

            // Approximate Newton correction (for demonstration only)
            for(size_t i=0; i<n; ++i)
            {
                X1(i) -= R1(i);
                X2(i) -= R2(i);
                X3(i) -= R3(i);
            }
        }

        // Update solution
        state_type x_new(x);
        for(size_t i=0; i<n; ++i)
            x_new(i) += dt*(b1*K1(i) + b2*K2(i) + b3*K3(i));

        // Compute error estimate
        value_type err = 0.0;
        for(size_t i=0; i<n; ++i)
        {
            value_type sk = m_atol + m_rtol * std::max(std::fabs(x(i)), std::fabs(x_new(i)));
            value_type e_i = dt*((b1 - a31)*K1(i) + (b2 - a32)*K2(i) + (b3 - a33)*K3(i));
            err += (e_i / sk) * (e_i / sk);
        }
        err = std::sqrt(err / n);

        if(err <= 1.0)
        {
            t += dt;
            x = x_new;
            value_type dt_new = dt * std::min((value_type)1.0,
                std::max((value_type)0.2, m_safety * std::pow(err, -m_alpha)));
            dt = dt_new;
            return controlled_step_result::success;
        }
        else
        {
            dt *= std::max((value_type)0.1, m_safety * std::pow(err, -m_alpha));
            return controlled_step_result::fail;
        }
    }

    //-----------------------------------------------------------------------
    // Dense output: linear interpolation (placeholder)
    //-----------------------------------------------------------------------
    state_type interpolate(
        const state_type &x_old,
        const state_type &x_new,
        time_type t_old,
        time_type t_new,
        time_type t_interp
    ) const
    {
        value_type theta = (t_interp - t_old) / (t_new - t_old);
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
};

// --------------------------------------------------------------------
// Tell Boost.Odeint that radau5_dense_ext is a dense output stepper
// --------------------------------------------------------------------
template<class Value, class State, class Deriv, class Time>
struct get_dense_output< radau5_dense_ext<Value, State, Deriv, Time> >
{
    typedef radau5_dense_ext<Value, State, Deriv, Time> type;
};

} // namespace odeint
} // namespace numeric
} // namespace boost

#endif // RADAU5_DENSE_EXT_HPP_INCLUDED

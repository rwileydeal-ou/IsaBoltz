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

        const value_type dt_min = 1e-16;
        const int max_retries = 5;
        const int max_newton = 7;
        const value_type newton_tol = 1e-5;
        const value_type eps_reg = 1e-12;
        const value_type max_dt = 1.0;

        // Radau IIA coefficients
        const value_type sq6 = std::sqrt(6.0);
        const value_type c[3] = { (4.0 - sq6)/10.0, (4.0 + sq6)/10.0, 1.0 };
        const value_type a[3][3] = {
            { (88.0 - 7.0*sq6)/360.0, (296.0 - 169.0*sq6)/1800.0, (-2.0 + 3.0*sq6)/225.0 },
            { (296.0 + 169.0*sq6)/1800.0, (88.0 + 7.0*sq6)/360.0, (-2.0 - 3.0*sq6)/225.0 },
            { (16.0 - sq6)/36.0, (16.0 + sq6)/36.0, 1.0/9.0 }
        };
        const value_type b[3] = { a[2][0], a[2][1], a[2][2] };
        const value_type b_hat[3] = { 0.19681547722366, 0.39442431473909, 0.21132486540519 };

        // Stage storage (preallocated)
        state_type K[3] = { state_type(n, 0.0), state_type(n, 0.0), state_type(n, 0.0) };
        state_type F[3] = { state_type(n, 0.0), state_type(n, 0.0), state_type(n, 0.0) };

        // Block Jacobian matrix (preallocated)
        matrix_type M(3*n, 3*n);
        vector<value_type> rhs(3*n);

        bool step_success = false;
        bool jacobian_needs_update = true;

        for(int retry = 0; retry < max_retries; ++retry)
        {
            bool newton_failed = false;
            
            state_type f0(n);
            system(x, f0, t);

            // Initialize stages to previous solution
            for(int i=0; i<3; ++i){
                K[i]=f0;
            }

            // Update Jacobian only if necessary
            if(jacobian_needs_update)
            {
                jacobian(x, m_J, t); // m_J is cached
            }

            // Build 3n x 3n Newton matrix (I - dt*A⊗J)
            for(int i=0; i<3; ++i)
                for(int j=0; j<3; ++j)
                {
                    matrix_type block = -dt * a[i][j] * m_J;
                    if(i==j) for(size_t k=0;k<n;++k) block(k,k) += 1.0 + eps_reg;

                    for(size_t r=0;r<n;++r)
                        for(size_t c0=0;c0<n;++c0)
                            M(i*n + r, j*n + c0) = block(r,c0);
                }

            // LU factorization (reuse if step size unchanged and Jacobian unchanged)
            matrix_type M_copy = M;
            permutation_matrix<std::size_t> pm(3*n);
            if(lu_factorize(M_copy, pm) != 0)
            {
                dt *= 0.5;
                jacobian_needs_update = true;
                continue;
            }

            // Newton iterations
            value_type prev_res = 0.0;
            for(int iter = 0; iter < max_newton; ++iter)
            {
                // Compute residuals F_i = K_i - f(t + c_i*dt, x + dt*sum_j a_ij*K_j)
                for(int i=0;i<3;++i)
                {
                    state_type y = x;
                    for(int j=0;j<3;++j) y += dt * a[i][j] * K[j];
                    system(y, F[i], t + c[i]*dt);
                    for(size_t k=0;k<n;++k) F[i](k) = K[i](k) - F[i](k);
                }

                // Assemble RHS
                for(int i=0;i<3;++i)
                    for(size_t k=0;k<n;++k)
                        rhs(i*n + k) = -F[i](k);

                // Solve linear system (reuse LU)
                vector<value_type> deltaK = rhs;
                lu_substitute(M_copy, pm, deltaK);

                // Update stages
                for(int i=0;i<3;++i)
                    for(size_t k=0;k<n;++k)
                        K[i](k) += deltaK(i*n + k);

                // Check convergence
                value_type max_res = 0.0;
                for(int i=0;i<3;++i)
                    for(size_t k=0;k<n;++k)
                        max_res = std::max(max_res, std::fabs(F[i](k)));
                if(max_res < newton_tol) break;

                // If residual stagnates, rebuild M and refactorize
                if (iter > 0 && max_res > 0.9 * prev_res)
                {
                    jacobian(x, m_J, t);

                    // Rebuild block matrix with same dt
                    for(int i=0; i<3; ++i)
                        for(int j=0; j<3; ++j)
                        {
                            matrix_type block = -dt * a[i][j] * m_J;
                            if(i==j) for(size_t k=0;k<n;++k) block(k,k) += 1.0 + eps_reg;
                            for(size_t r=0;r<n;++r)
                                for(size_t c0=0;c0<n;++c0)
                                    M(i*n + r, j*n + c0) = block(r,c0);
                        }

                    M_copy = M;
                    if(lu_factorize(M_copy, pm) != 0)
                    {
                        newton_failed = true;
                        break;
                    }
                }

                prev_res = max_res;

            }

            if(newton_failed)
            {
                dt *= 0.5;
                jacobian_needs_update = true;
                continue;
            }

            // Compute new solution
            state_type x_new = x;
            for(int i=0;i<3;++i) x_new += dt * b[i] * K[i];

            // Embedded error estimation (4th-order)
            value_type err = 0.0;
            for(size_t k=0;k<n;++k)
            {
                value_type sk = m_atol + m_rtol * std::max(std::fabs(x(k)), std::fabs(x_new(k)));
                value_type e_i = dt*((b[0]-b_hat[0])*K[0](k) + (b[1]-b_hat[1])*K[1](k) + (b[2]-b_hat[2])*K[2](k));
                err += (e_i/sk)*(e_i/sk);
            }
            err = std::sqrt(err/n);

            if(err <= 1.0)
            {
                // Accept step
                t += dt;
                x = x_new;
                const value_type max_growth = 10.0;
                const value_type min_shrink = 0.1;
                value_type dt_scale = m_safety * std::pow(err, -m_alpha);
                dt_scale = std::min(max_growth, std::max(min_shrink, dt_scale));
                dt *= dt_scale;
                dt = std::min(dt, max_dt);

                step_success = true;
                jacobian_needs_update = false;
                break;
            }
            else
            {
                dt *= std::max((value_type)0.1, m_safety * std::pow(err, -m_alpha));
                jacobian_needs_update = true;
            }
        }

        if(!step_success){
            if(dt < dt_min) 
                throw std::runtime_error("Radau5 step size too small.");

            return controlled_step_result::fail;
        }

        return controlled_step_result::success;
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

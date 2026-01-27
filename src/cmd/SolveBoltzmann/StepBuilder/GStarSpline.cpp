#include <cmd/SolveBoltzmann/StepBuilder/GStarSpline.h>
#include <ModelIndependent/GStar.h>
#include <stdexcept>
#include <iostream>

using std::log;
using std::exp;

std::unique_ptr<GStarSpline> GStarSpline::instance_ = nullptr;

const GStarSpline& GStarSpline::instance() {
    if (!instance_)
        instance_.reset(new GStarSpline());   // direct new, NOT make_unique

    return *instance_;
}

GStarSpline::GStarSpline() {
    buildTable();
    buildSpline();
}

const std::vector<Models::Particle>* GStarSpline::particles_ptr_ = nullptr;
Connection* GStarSpline::connection_ptr_ = nullptr;

void GStarSpline::initialize(
    const std::vector<Models::Particle>& particles,
    Connection& connection)
{
    particles_ptr_  = &particles;
    connection_ptr_ = &connection;
}

void GStarSpline::buildTable() {
    table_.reserve(N);

    // log-spaced temperature grid
    const double logTmin = std::log(Tmin);
    const double logTmax = std::log(Tmax);
    const double dlogT = (logTmax - logTmin) / (N - 1);

    for (int i = 0; i < N; ++i) {
        double T = std::exp(logTmin + i * dlogT);

        // heavy cost only here:
        if (!particles_ptr_ || !connection_ptr_)
            throw std::runtime_error("GStarSpline used before initialization");

        double g  = GStar::Calculate(*particles_ptr_, *connection_ptr_, T);
        double gs = GStar::CalculateEntropic(*particles_ptr_, *connection_ptr_, T);
        table_.push_back({T, g, gs});
    }
}

// Build monotonic, C^1 cubic Hermite spline coefficients
void GStarSpline::buildSpline() {
    int M = table_.size();

    g_a_.resize(M);
    g_b_.resize(M);
    g_c_.resize(M);

    gs_a_.resize(M);
    gs_b_.resize(M);
    gs_c_.resize(M);

    for (int i = 0; i < M; ++i) {
        g_a_[i] = table_[i].g;
        gs_a_[i] = table_[i].gs;
    }

    // Compute finite-difference slopes for Hermite spline
    for (int i = 0; i < M - 1; ++i) {
        double h = table_[i+1].T - table_[i].T;
        double dg = (table_[i+1].g - table_[i].g) / h;
        double dgs = (table_[i+1].gs - table_[i].gs) / h;

        g_b_[i] = dg;
        gs_b_[i] = dgs;
    }

    // Set endpoint slopes to match neighbors
    g_b_[M-1] = g_b_[M-2];
    gs_b_[M-1] = gs_b_[M-2];
}

double GStarSpline::hermiteEval(double x, double x0, double x1,
                                double y0, double y1,
                                double m0, double m1)
{
    double h = x1 - x0;
    if (h <= 0) return y0;

    double t = (x - x0) / h;
    double t2 = t*t;
    double t3 = t2*t;

    return (2*t3 - 3*t2 + 1)*y0
         + (t3 - 2*t2 + t)*m0*h
         + (-2*t3 + 3*t2)*y1
         + (t3 - t2)*m1*h;
}

double GStarSpline::g(double T) const {
    if (T <= table_.front().T) return table_.front().g;
    if (T >= table_.back().T) return table_.back().g;

    // binary search
    int i = std::lower_bound(
        table_.begin(), table_.end(), T,
        [](auto& a, double x){ return a.T < x; }
    ) - table_.begin() - 1;

    double x0 = table_[i].T;
    double x1 = table_[i+1].T;

    return hermiteEval(
        T, x0, x1,
        g_a_[i], g_a_[i+1],
        g_b_[i], g_b_[i+1]
    );
}

double GStarSpline::gs(double T) const {
    if (T <= table_.front().T) return table_.front().gs;
    if (T >= table_.back().T) return table_.back().gs;

    int i = std::lower_bound(
        table_.begin(), table_.end(), T,
        [](auto& a, double x){ return a.T < x; }
    ) - table_.begin() - 1;

    double x0 = table_[i].T;
    double x1 = table_[i+1].T;

    return hermiteEval(
        T, x0, x1,
        gs_a_[i], gs_a_[i+1],
        gs_b_[i], gs_b_[i+1]
    );
}

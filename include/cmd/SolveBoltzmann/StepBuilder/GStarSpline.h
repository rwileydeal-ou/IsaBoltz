#pragma once
#include <deque>
#include <vector>
#include <cmath>
#include <mutex>
#include <memory>
#include <algorithm>
#include <Models/Particle.h>
#include <Models/Connection.h>

class GStarSpline {
public:
    struct Node {
        double T;
        double g;
        double gs;
    };

    // Singleton accessor
    static const GStarSpline& instance();
    static void initialize(const std::deque<Models::Particle>& particles,
                           Connection& connection);

    // Evaluate Gstar(T)
    double g(double T) const;

    // Evaluate Gstar_entropic(T)
    double gs(double T) const;

private:
    GStarSpline();  // builds the table & spline
    void buildTable();
    void buildSpline();

    static std::unique_ptr<GStarSpline> instance_;

    std::vector<Node> table_;

    static const std::deque<Models::Particle>* particles_ptr_;
    static Connection* connection_ptr_;
    
    // Spline coefficients
    std::vector<double> g_a_, g_b_, g_c_;     // cubic Hermite coefficients
    std::vector<double> gs_a_, gs_b_, gs_c_;

    static constexpr int N = 800;             // number of nodes
    static constexpr double Tmin = 1e-12;
    static constexpr double Tmax = 1e4;

    static double hermiteEval(double x, double x0, double x1,
                              double y0, double y1,
                              double m0, double m1);

    mutable std::once_flag built_;
};

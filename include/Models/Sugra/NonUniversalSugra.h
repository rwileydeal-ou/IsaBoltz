#include <boost/flyweight.hpp>

struct GenerationsOneAndTwo{
    bool GenOneTwo;
    double m_uL_GUT;
    double m_dR_GUT;
    double m_uR_GUT;
    double m_eL_GUT;
    double m_eR_GUT;
};

struct GenerationThree{
    bool GenThree;
    double m_lL_GUT;
    double m_lR_GUT;
    double m_bR_GUT;
    double m_tL_GUT;
    double m_tR_GUT;
};

struct WeakInputs{
    bool WeakInput;
    double weakMu;
    double weakMA;
};

struct NonUniversalSugraOptions{
    bool Gaugino;
    bool Trilinear;
    bool Higgs;
    GenerationsOneAndTwo GenOneTwo;
    GenerationThree GenThree;
    WeakInputs WeakInput;
    bool RightNeutrino;
    double LambdaRPV;
};

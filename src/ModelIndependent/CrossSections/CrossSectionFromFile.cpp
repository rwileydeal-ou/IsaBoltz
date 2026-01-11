#include <ModelIndependent/CrossSections/CrossSectionFromFile.h>

CrossSectionFromFile::CrossSectionFromFile(std::string fileName)
    : fileName_(std::move(fileName))
{
    std::ifstream in(fileName_);
    if (!in.is_open())
        throw std::runtime_error("Cannot open cross-section file: " + fileName_);

    std::string line;
    int tempCol = -1, sigCol = -1;

    // Parse header
    if (!std::getline(in, line))
        throw std::runtime_error("Empty sigmaV file");

    {
        boost::tokenizer<boost::char_separator<char>> tok(line, boost::char_separator<char>(","));
        int i = 0;
        for (auto& col : tok) {
            if (col == "Temperature") tempCol = i;
            if (col == "SigmaV")      sigCol = i;
            ++i;
        }
    }

    if (tempCol < 0 || sigCol < 0)
        throw std::runtime_error("Could not find Temperature or SigmaV columns");

    // Read rows
    while (std::getline(in, line)) {
        boost::tokenizer<boost::char_separator<char>> tok(line, boost::char_separator<char>(","));
        int i = 0;
        double T = 0.0;
        double s = 0.0;

        for (auto& col : tok) {
            if (i == tempCol) T = std::stod(col);
            if (i == sigCol)  s = std::stod(col);
            ++i;
        }
    
        temps_.push_back(T);
        sigmas_.push_back(s);
    }

    std::reverse(temps_.begin(), temps_.end());
    std::reverse(sigmas_.begin(), sigmas_.end());

    // Ensure monotonicity for interpolation
    if (!std::is_sorted(temps_.begin(), temps_.end()))
        throw std::runtime_error("Temperature column is not sorted");
}
CrossSectionFromFile::~CrossSectionFromFile(){

}

SigmaV CrossSectionFromFile::Calculate(
    const Models::Particle& particle,
    const ModelBase& model,
    double T,
    std::shared_ptr<DataRelay>
){
    if (T <= 0.0)
        return SigmaV(particle, T, 0.0);

    auto it = std::upper_bound(temps_.begin(), temps_.end(), T);
    size_t idx = it - temps_.begin();

    double T1 = temps_[idx - 1];
    double T2 = temps_[idx];
    double s1 = sigmas_[idx - 1];
    double s2 = sigmas_[idx];

    double sigma = s1 + (s2 - s1) * (T - T1) / (T2 - T1);

    return SigmaV(particle, T, sigma);
}

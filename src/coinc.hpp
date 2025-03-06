#ifndef COINC_HPP
#define COINC_HPP

#include "single.hpp"

#include <vector>
#include <tuple>

class CoincidenceSpectrum {
private:
    std::vector<std::vector<unsigned int>> hist;

    std::tuple<std::tuple<double, double>, std::tuple<double, double>> ecal;
    std::tuple<std::tuple<int, int>, std::tuple<int, int>> window;

    std::vector<SingleSpectrum*> parents;

    double s, ds, w, dw, v2p, dv2p, dcounts, dpeak_counts, peak_center;
    unsigned int counts = 0, peak_counts = 0;

public:
    CoincidenceSpectrum(
        std::vector<std::vector<unsigned int>>&& hist,
        std::tuple<std::tuple<double, double>, std::tuple<double, double>> ecal = {},
        std::tuple<std::tuple<int, int>, std::tuple<int, int>> window = {},
        std::vector<SingleSpectrum*> parents = {});
};

#endif
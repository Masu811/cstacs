#ifndef SINGLE_HPP
#define SINGLE_HPP

#include <string>
#include <vector>
#include <tuple>
#include <cmath>

inline unsigned int searchsorted(const std::vector<double>& array, double value);

class SingleSpectrum {
private:
    const std::string detname, filename;
    bool show, verbose, autocompute;

    std::vector<unsigned int> spectrum;
    std::tuple<double, double> ecal;
    double eres;

    std::vector<double> energies, peak, peak_energies;

    double s_width = 1.1, w_width = 1.0, w_dist = 3.0, peak_width = 60;
    std::tuple<double, double, double, double> v2p_bounds = {450, 490, 506, 516};

    double s, ds, w, dw, v2p, dv2p, dcounts, dpeak_counts, peak_center;
    unsigned int counts = 0, peak_counts = 0;

public:
    SingleSpectrum(
        std::vector<unsigned int>&& spectrum,
        std::string detname = "",
        std::tuple<double, double> ecal = {},
        double eres = 0,
        std::string filename = "",
        bool show = false,
        bool verbose = false,
        bool autocompute = true);

    void analyze(
        const double s_width = 1.1,
        const double w_width = 1.0,
        const double w_dist = 3.0,
        const double peak_width = 60,
        const std::tuple<double, double, double, double> v2p_bounds = {450, 490, 506, 516},
        const bool bg_corr = true,
        const double bg_frac = 0.25,
        const bool follow_peak = true,
        const int follow_peak_order = 1);
};

#endif
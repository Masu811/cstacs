#include "single.hpp"

#include <cmath>
#include <cstdarg>

#include <iostream>
#include <string>
#include <vector>
#include <tuple>
#include <algorithm>
#include <numeric>
#include <fstream>

extern "C" {
    #include "fitting_functions.h"
}


#define M_POSITRON_keV 510.99895

#define DEBUG_MODE

#ifdef DEBUG_MODE
    #define DEBUG(x) std::cout << x << std::endl
#else
    #define DEBUG(x)
#endif


inline unsigned int searchsorted(const std::vector<double>& array, double value) {
    return std::distance(array.begin(), std::lower_bound(array.begin(), array.end(), value));
}


template <typename T, typename... Args>
void dump(const std::string& filename, const std::vector<T>& first,
          const std::vector<Args>&... rest)
{
    int n_cols = sizeof...(rest);
    const size_t size = first.size();
    if (n_cols > 0 && ((rest.size() != size) || ...)) {
        std::cerr << "Attempting to dump vectors of different size. Aborting..." << std::endl;
        return;
    }

    std::ofstream file(filename);
    if (!file) {
        std::cerr << "Could not open file " << filename << std::endl;
        return;
    }

    for (int i = 0; i < size; i++) {
        file << first[i];
        ((file << "," << rest[i]), ...);
        file << "\n";
    }

    file.close();
}


SingleSpectrum::SingleSpectrum(
    std::vector<unsigned int>&& spectrum,
    std::string detname,
    std::tuple<double, double> ecal,
    double eres,
    std::string filename,
    bool show,
    bool verbose,
    bool autocompute)
    : spectrum(spectrum),
      detname(detname),
      ecal(ecal),
      eres(eres),
      filename(filename),
      show(show),
      verbose(verbose),
      autocompute(autocompute)
{
    counts = std::accumulate(spectrum.begin(), spectrum.end(), 0);
    dcounts = std::sqrt(counts);

    if (autocompute) analyze();
}


void SingleSpectrum::analyze(
    const double s_width,
    const double w_width,
    const double w_dist,
    const double peak_width,
    const std::tuple<double, double, double, double> v2p_bounds,
    const bool bg_corr,
    const double bg_frac,
    const bool follow_peak,
    const int follow_peak_order)
{
    #ifdef DEBUG_MODE
        verbose = true;
    #endif

    if (verbose) std::cout << "==== Analyzing SingleSpectrum of detector " << detname << " ====\n";

    // Ecal check and energy calculation

    double a = std::get<0>(ecal);
    double b = std::get<1>(ecal);

    DEBUG("Ecal: const: " << a << ", lin: " << b);

    if (std::isnan(a) || std::isnan(b) || b == 0) {
        std::cerr << "SingleSpectrum::analyze() called on object without "
                  << "energy calibration. Aborting..." << std::endl;
        return;
    } else {
        DEBUG("Calculating energies... ");
        for (unsigned int i = 0; i < spectrum.size(); i++) {
            energies.push_back(a + b * i);
        }
    }

    // Attribute update

    this->s_width = s_width;
    this->w_width = w_width;
    this->w_dist = w_dist;
    this->peak_width = peak_width;
    this->v2p_bounds = v2p_bounds;

    // Peak extraction

    double lower_peak_area_energy = M_POSITRON_keV - peak_width / 2;
    double upper_peak_area_energy = M_POSITRON_keV + peak_width / 2;

    DEBUG("Extracting peak... ");

    unsigned int lower_peak_area_idx = searchsorted(energies, lower_peak_area_energy);
    unsigned int upper_peak_area_idx = searchsorted(energies, upper_peak_area_energy);

    DEBUG("Lower peak idx: " << lower_peak_area_idx << ", upper peak idx: " << upper_peak_area_idx);

    peak = std::vector<double>(spectrum.begin() + lower_peak_area_idx,
                               spectrum.begin() + upper_peak_area_idx);

    DEBUG("Calculating peak counts... ");

    peak_counts = std::accumulate(peak.begin(), peak.end(), 0);

    double mean_counts_in_peak = peak_counts / peak.size();

    double std_counts_in_peak = std::accumulate(peak.begin(), peak.end(), 0,
                        [mean_counts_in_peak](double std, double val) {
                            return std + std::abs(mean_counts_in_peak - val);
                        }) / peak.size();

    if (std_counts_in_peak < 1) {
        std::cerr << "No peak found. Maybe your energy calibration is faulty? "
                  << "Could not analyze the single spectrum of detector "
                  << detname << " (no S, W & V2P parameters calculated).";
        return;
    }

    DEBUG("Extracting peak energies...");

    peak_energies = std::vector<double>(energies.begin() + lower_peak_area_idx,
                                        energies.begin() + upper_peak_area_idx);

    DEBUG("Extracted peak energies min: " << *peak_energies.begin() << ", max: " << peak_energies.back());

    // Initial guesses for first Gauss fit

    DEBUG("Making initial guess for Gauss fit...");

    auto peak_max = std::max_element(peak.begin(), peak.end());
    unsigned int peak_max_idx = std::distance(peak.begin(), peak_max);
    double peak_center_guess = peak_energies.at(peak_max_idx);

    std::cout << "Initial peak center guess: " << peak_center_guess  << ", Counts: " << *peak_max << std::endl;

    constexpr double inv_sig = 2.3548 / 3.3;

    // gauss: amp, x0, inv_sig
    double gauss_params[3] = {*peak_max, peak_center_guess, inv_sig};

    if (verbose) std::cout << "Performing first Gauss fit..." << std::endl;

    fitGaussian(peak_energies.data(), peak.data(), peak.size(), gauss_params);

    peak_center = gauss_params[1];

    if (verbose) {
        std::cout << "Peak center " << peak_center << " keV deviates from " << M_POSITRON_keV
                  << " keV by " << peak_center - M_POSITRON_keV
                  << " keV (< 0.5 keV is typically okay).\n";
    }

    if (bg_corr) {
        if (verbose) std::cout << "Performing background subtraction..." << std::endl;

        double bg_width = bg_frac * peak_width / 2;

        DEBUG("Getting background indices...");

        unsigned int left_bg_right_edge_idx = searchsorted(peak_energies, M_POSITRON_keV - peak_width / 2 + bg_width);
        unsigned int right_bg_left_edge_idx = searchsorted(peak_energies, M_POSITRON_keV + peak_width / 2 - bg_width);

        std::vector<double> bg_energies, bg;

        unsigned int n_bg_elements = left_bg_right_edge_idx + peak.end() - peak.begin() - right_bg_left_edge_idx;

        bg_energies.reserve(n_bg_elements);
        bg.reserve(n_bg_elements);

        DEBUG("Extracting background arrays...");

        bg_energies.insert(bg_energies.end(), peak_energies.begin(), peak_energies.begin() + left_bg_right_edge_idx);
        bg_energies.insert(bg_energies.end(), peak_energies.begin() + right_bg_left_edge_idx, peak_energies.end());
        bg.insert(bg.end(), peak.begin(), peak.begin() + left_bg_right_edge_idx);
        bg.insert(bg.end(), peak.begin() + right_bg_left_edge_idx, peak.end());

        auto bg_max = std::max_element(bg.begin(), bg.end());
        auto bg_min = std::min_element(bg.begin(), bg.end());

        DEBUG("Extracted background max: " << *bg_max << ", min: " << *bg_min);

        constexpr double inv_sig_erf = 2.3548 / 1.5;

        // erf: amp, x0, inv_sig, offset
        double erf_params[4] = {*bg_min - *bg_max, peak_center, inv_sig_erf, *bg_min};

        if (verbose) std::cout << "Performing background erf fit..." << std::endl;

        fitErf(bg_energies.data(), bg.data(), n_bg_elements, erf_params);

        // gauss_erf: gauss_amp, x0, gauss_inv_sig, erf_amp, erf_inv_sig, offset
        double gauss_erf_params[6] = {gauss_params[0], gauss_params[1], gauss_params[2],
                                      erf_params[0], inv_sig_erf, erf_params[3]};

        if (verbose) std::cout << "Performing combined erf and Gauss fit..." << std::endl;

        fitGaussErf(peak_energies.data(), peak.data(), peak.size(), gauss_erf_params);

        if (verbose) std::cout << "All fitting routines have completed\n"
                               << "Performing background subtraction..." << std::endl;

        #ifdef DEBUG_MODE
            dump("./uncorr_peak_" + detname + ".csv", peak_energies, peak);
        #endif

        double fit;
        DEBUG("gauss_erf params:");
        DEBUG("\tgauss_amp: " << gauss_erf_params[0] << ", x0: " << gauss_erf_params[1]
                << ", gauss_inv_sig: " << gauss_erf_params[2]);
        DEBUG("\terf_amp: " << gauss_erf_params[3] << ", erf_inv_sig: " << gauss_erf_params[4]
                << ", offset: " << gauss_erf_params[5]);

        for (int i = 0; i < peak.size(); i++) {
            peak[i] -= my_erf(peak_energies[i], gauss_erf_params[3], gauss_erf_params[1],
                              gauss_erf_params[4], gauss_erf_params[5]);
        }

        #ifdef DEBUG_MODE
            dump("./corr_peak_" + detname + ".csv", peak_energies, peak);
        #endif

        double corr_peak_counts = std::accumulate(peak.begin(), peak.end(), 0);

        if (verbose) std::cout << "Background subtraction done (Subtracted "
                               << (1 - corr_peak_counts / peak_counts) * 100
                               << " % of peak counts)\n";

        peak_counts = corr_peak_counts;
        peak_center = gauss_erf_params[1];
    }
}
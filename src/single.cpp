#include "single.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <tuple>
#include <cmath>
#include <algorithm>
#include <numeric>

extern "C" {
    #include "fitting_functions.h"
}


#define M_POSITRON_keV 510.99895


inline unsigned int searchsorted(const std::vector<double>& array, double value) {
    return std::distance(array.begin(), std::lower_bound(array.begin(), array.end(), value));
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
    const bool bg_corr)
{
    // Ecal check and energy calculation

    double a = std::get<0>(ecal);
    double b = std::get<1>(ecal);

    if (std::isnan(a) || std::isnan(b)) {
        std::cerr << "SingleSpectrum::analyze() called on object without "
                  << "energy calibration. Aborting..." << std::endl;
        return;
    } else {
        for (size_t i = 0; i < spectrum.size(); i++) {
            energies.push_back(a + b * i);
        }
    }

    // Attribute update

    this->s_width = s_width;
    this->w_width = w_width;
    this->w_dist = w_dist;
    this->peak_width = peak_width;
    this->v2p_bounds = v2p_bounds;

    if (verbose) {
        std::cout << "Analyzing SingleSpectrum of detector " << detname << "...\n";
    }

    // Peak extraction

    double lower_peak_area_energy = M_POSITRON_keV - peak_width / 2;
    double upper_peak_area_energy = M_POSITRON_keV + peak_width / 2;

    size_t lower_peak_area_idx = searchsorted(energies, lower_peak_area_energy);
    size_t upper_peak_area_idx = searchsorted(energies, upper_peak_area_energy);

    peak = std::vector<double>(spectrum.begin() + lower_peak_area_idx,
                                spectrum.begin() + upper_peak_area_idx);

    double mean_counts_in_peak = std::accumulate(peak.begin(), peak.end(), 0) / peak.size();

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

    peak_energies = std::vector<double>(energies.begin() + lower_peak_area_idx,
                                        energies.begin() + upper_peak_area_idx);

    // Initial guesses for first Gauss fit

    auto peak_max = std::max_element(peak.begin(), peak.end());
    size_t peak_max_idx = std::distance(peak.begin(), peak_max);
    double peak_center = peak_energies.at(peak_max_idx);

    // NOTE: indexing into peak_max is dangerous
    double init[3] = {*peak_max, peak_center, 1.5};

    if (verbose) {
        std::cout << "Performing first Gauss fit..." << std::endl;
    }

    // contains amp, pos and sig of peak
    double *result = fitGaussian(peak_energies.data(), peak.data(), peak.size(), init);

    peak_center = result[1];

    free(result);

    if (verbose) {
        std::cout << "Peak center " << peak_center << " keV deviates from " << M_POSITRON_keV
                  << "keV by " << peak_center - M_POSITRON_keV
                  << " keV (< 0.5 keV is typically okay).\n";
    }

    if (bg_corr) {

    }
}
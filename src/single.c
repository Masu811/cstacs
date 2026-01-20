#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "../include/fitting.h"
#include "../include/single.h"
#include "../include/structs.h"

extern int verbose;
extern int debug;

typedef struct {
    int *spectrum;
    double *energies;
    int size;
    double *corr_spectrum;
} SpectrumView;

/*
 * Calculate energies of the channels of SingleSpectrum s with its energy
 * calibration.
 * Return Codes:
 * 0: Success
 * 1: Failure (Missing ecal or malloc fail)
 */
static int calcEnergies(SingleSpectrum *s) {
    if (s == NULL) {
        puts("Cannot calculate energies of uninitialized Single Spectrum");
        return 1;
    }

    if (s->ecal_found == false) {
        puts("Single Spectrum is missing energy calibration");
        return 1;
    }

    double *energies = (double *)malloc(s->spectrum_size * sizeof(double));

    if (energies == NULL) {
        puts("Memory allocation for energy array failed");
        return 1;
    }

    for (int i = 0; i < s->spectrum_size; i++) {
        energies[i] = (s->ecal.values[0] + i * s->ecal.values[1]);
    }

    s->energies = energies;

    return 0;
}

int singleComplete(SingleSpectrum *s) {
    if (s == NULL) {
        puts("Cannot analyze uninitialized Single Spectrum");
        return 0;
    }

    if (s->spectrum == NULL) {
        puts("Cannot analyze empty Single Spectrum");
        return 0;
    }

    if (s->ecal_found == false) {
        puts("Single Spectrum is missing energy calibration");
        return 0;
    }

    if (s->energies == NULL && calcEnergies(s) != 0) {
        return 0;
    }

    return 1;
}

double *intToDouble(int *arr, const size_t n) {
    double *x = (double*)malloc(n * sizeof(double));

    for (int i = 0; i < n; i++) {
        x[i] = (double)arr[i];
    }

    return x;
}

int getPeakBndIdx(SingleSpectrum *s, double peak_width, int direction) {
    int peak_bnd_idx;

    double h = direction * peak_width / 2;
    double peak_bnd_x = (
        ((M_POSITRON_keV + h) - s->ecal.values[0]) / s->ecal.values[1]
    );

    if (peak_bnd_x < 0) {
        peak_bnd_idx = 0;
    } else if (peak_bnd_x > s->spectrum_size - 1) {
        peak_bnd_idx = s->spectrum_size - 1;
    } else {
        peak_bnd_idx = round(peak_bnd_x);
    }

    return peak_bnd_idx;
}

// Return codes:
// 0: Success
// 1: Failure
int extractPeak(SingleSpectrum *s, SpectrumView *peak, double peak_width) {
    if (peak_width == 0) {
        return 1;
    }

    int left_peak_bnd_idx = getPeakBndIdx(s, peak_width, -1);
    int right_peak_bnd_idx = getPeakBndIdx(s, peak_width, 1);

    int n_ch_in_peak = right_peak_bnd_idx - left_peak_bnd_idx + 1;

    if (n_ch_in_peak < 3) {
        return 1;
    }

    if (verbose) {
        printf("Extracted peak with %d channels width.\n", n_ch_in_peak);
    }

    peak->spectrum = &s->spectrum[left_peak_bnd_idx];
    peak->energies = &s->energies[left_peak_bnd_idx];
    peak->size = n_ch_in_peak;

    for (int i = 0; i < peak->size; i++) {
        peak->corr_spectrum[i] = peak->spectrum[i];
    }

    return 0;
}

// Return values:
// 0: Success
// 1: Failure
int checkPeakStd(SpectrumView *peak, char *detname) {
    if (peak == NULL || peak->size < 3) {
        return 1;
    }

    double mean = 0, diff, std = 0;

    for (int i = 0; i < peak->size; i++) {
        mean += peak->spectrum[i];
    }

    mean /= peak->size;

    for (int i = 0; i < peak->size; i++) {
        diff = peak->spectrum[i] - mean;
        std += diff * diff;
    }

    std = sqrt(std / (peak->size - 1));

    if (std < 1 || std == NAN) {
        printf(
            "No peak found. Maybe your energy calibration is faulty? "
            "Could not analyze the single spectrum of detector "
            "%s (no S, W & V2P parameters calculated).",
            detname == NULL ? "?" : detname
        );
        return 1;
    }

    return 0;
}

// Return values:
// 0: Success
// 1: Failure
int fitPeakGauss(
    double fit_result[3],
    SpectrumView *peak
) {
    double max = 0;
    int argmax = 0;

    for (int i = 0; i < peak->size; i++) {
        if (peak->spectrum[i] > max) {
            max = peak->spectrum[i],
            argmax = i;
        }
    }

    double x0 = peak->energies[argmax];
    double A_gauss = max;

    double init[5] = {
        A_gauss,
        x0,
        1.5,
    };

    if (debug) puts("####\nResults of Gaussian peak fit:");

    // contains amp, pos and sig of peak
    double *result = fitGaussian(
        peak->energies, peak->corr_spectrum, peak->size, init, debug
    );

    if (result == NULL) {
        return 1;
    }

    for (int i = 0; i < 3; i++) {
        fit_result[i] = result[i];
    }

    if (debug) {
        puts("####");

        double *fit = (double*)malloc(peak->size * sizeof(double));

        if (fit == NULL) {
            free(result);
            return 0;
        }

        for (int i = 0; i < peak->size; i++) {
            fit[i] = gaussian(
                peak->energies[i], result[0], result[1], result[2]
            );
        }

        double *columns[3] = {peak->energies, peak->corr_spectrum, fit};
        char *headers[3] = {"Energy", "Channel Data", "Best Fit"};

        // Dump data to file here

        free(fit);
    }

    free(result);
    return 0;
}

// Return values:
// 0: Success
// 1: Failure
int correctEcal(SingleSpectrum *s, SpectrumView *peak, int follow_peak_order) {
    double peak_fit_result[3];

    int fit_status = fitPeakGauss(peak_fit_result, peak);

    if (fit_status != 0) {
        printf("Gaussian Fit of spectrum failed\n");
        return 1;
    }

    double c0 = s->ecal.values[0], c1 = s->ecal.values[1];

    if (follow_peak_order == 0) {
        s->ecal.values[0] -= peak_fit_result[2] - M_POSITRON_keV;
        if (verbose) {
            printf(
                "Corrected 0th order ecal coefficient: %f -> %f",
                c0, s->ecal.values[0]
            );
        }
    } else {
        s->ecal.values[1] *= (
            (M_POSITRON_keV - c0) / (peak_fit_result[2] - c0)
        );
        if (verbose) {
            printf(
                "Corrected 1st order ecal coefficient: %f -> %f",
                c1, s->ecal.values[1]
            );
        }
    }

    int ecal_status = calcEnergies(s);

    if (ecal_status != 0) {
        return 1;
    }

    s->ecal_corrected = true;

    return 0;
}

// Return values:
// 0: Success
// 1: Failure
int fitPeakComb(
    double fit_result[5],
    SpectrumView *peak
) {
    double max = 0;
    int argmax = 0;

    for (int i = 0; i < peak->size; i++) {
        if (peak->spectrum[i] > max) {
            max = peak->spectrum[i],
            argmax = i;
        }
    }

    double x0 = peak->energies[argmax];
    double A_erf = peak->corr_spectrum[peak->size - 1] - peak->corr_spectrum[0];
    double offset = peak->corr_spectrum[peak->size - 1];
    double A_gauss = max - offset - A_erf / 2;

    double init[5] = {
        A_gauss,
        A_erf,
        x0,
        1.5,
        offset,
    };

    if (debug) puts("####\nResults of peak fit:");

    // contains amp, pos and sig of peak
    double *result = fitComb(
        peak->energies, peak->corr_spectrum, peak->size, init, debug
    );

    if (result == NULL) {
        return 1;
    }

    for (int i = 0; i < 5; i++) {
        fit_result[i] = result[i];
    }

    if (debug) {
        puts("####");

        double *fit = (double*)malloc(peak->size * sizeof(double));

        if (fit == NULL) {
            free(result);
            return 0;
        }

        for (int i = 0; i < peak->size; i++) {
            fit[i] = comb(
                peak->energies[i],
                result[0], result[1], result[2], result[3], result[4]
            );
        }

        double *columns[3] = {peak->energies, peak->corr_spectrum, fit};
        char *headers[3] = {"Energy", "Channel Data", "Best Fit"};

        // Dump data to file here

        free(fit);
    }

    free(result);
    return 0;
}

int correctPeak(SpectrumView *peak) {
    double peak_fit_result[5];
    if (fitPeakComb(peak_fit_result, peak) != 0) {
        printf("Could not fit peak\n");
        return 2;
    }

    if (verbose) {
        printf(
            "Peak center %.3f keV deviates from %.3f "
            "keV by %.3f keV (< 0.5 keV is typically okay).\n",
            peak_fit_result[2],
            M_POSITRON_keV,
            peak_fit_result[2] - M_POSITRON_keV
        );
    }

    for (int i = 0; i < peak->size; i++) {
        peak->corr_spectrum[i] -= mod_erf(
            peak->energies[i],
            peak_fit_result[1],
            peak_fit_result[2],
            peak_fit_result[3],
            peak_fit_result[4]
        );
    }

    return 0;
}

double integrateSpectrum(
    void *s,
    double left_e,
    double right_e
) {
    // TODO: implement
    return 0;
}

void calcLineshapeParams(
    SingleSpectrum *s,
    SpectrumView *peak,
    double s_width,
    double w_width,
    double w_dist,
    bool w_rightonly,
    double v2p_bounds[4]
) {
    double s_counts, w_counts, w_l_counts, w_r_counts, peak_counts, valley_counts;

    s_counts = integrateSpectrum(
        &peak, M_POSITRON_keV - s_width, M_POSITRON_keV + s_width
    );

    w_r_counts = integrateSpectrum(
        &peak, M_POSITRON_keV - w_width - w_dist, M_POSITRON_keV - w_dist
    );
    w_l_counts = w_rightonly ? 0 : integrateSpectrum(
        &peak, M_POSITRON_keV - w_width - w_dist, M_POSITRON_keV - w_dist
    );
    w_counts = w_r_counts + w_l_counts;

    valley_counts = integrateSpectrum(
        s, v2p_bounds[0], v2p_bounds[1]
    );

    s->s = s_counts / peak_counts;
    s->w = w_counts / peak_counts;
    s->v2p = valley_counts / peak_counts;

    // TODO: uncertainties
}

/*
 * Calculate line shape parameters for SingleSpectrum.
 * Return codes:
 * 0: Success
 * 1: SingleSpectrum is missing some data for some calculations.
 * 2: No peak found
 * 3: Memory allocation failed
 */
int analyze(
    SingleSpectrum *s,
    const double s_width,
    const double w_width,
    const double w_dist,
    const bool w_rightonly,
    const double peak_width,
    const double bg_frac,
    const bool bg_corr,
    double v2p_bounds[4],
    const int follow_peak_order
) {
    if (!singleComplete(s)) return 1;

    if (verbose) {
        printf("Analyzing Single Spectrum of detector %s...\n", s->detname);
    }

    int status;

    SpectrumView peak;
    peak.corr_spectrum = (double*)malloc(peak.size * sizeof(double));

    if (peak.corr_spectrum == NULL) {
        return 3;
    }

    status = extractPeak(s, &peak, peak_width);

    if (status != 0) {
        free(peak.corr_spectrum);
        return 1;
    }

    status = checkPeakStd(&peak, s->detname);

    if (status != 0) {
        free(peak.corr_spectrum);
        return 2;
    }

    if ((follow_peak_order == 0 || follow_peak_order == 1) && !s->ecal_corrected) {
        status = correctEcal(s, &peak, follow_peak_order);

        if (status != 0) {
            free(peak.corr_spectrum);
            return 1;
        }

        status = extractPeak(s, &peak, peak_width);

        if (status != 0) {
            free(peak.corr_spectrum);
            return 1;
        }
    }

    if (bg_corr) {
        status = correctPeak(&peak);
    }

    calcLineshapeParams(
        s, &peak, s_width, w_width, w_dist, w_rightonly, v2p_bounds
    );

    free(peak.corr_spectrum);

    return 0;
}

#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "../include/fitting.h"
#include "../include/single.h"
#include "../include/structs.h"

extern int verbose;
extern int debug;

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

    for (int i = 0; i < s->spectrum_size; i++)
        energies[i] = (s->ecal.values[0] + i * s->ecal.values[1]);

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

    if (s->energies == NULL) {
        puts("Single Spectrum is missing energy calibration");
        return 0;
    }

    return 1;
}

double *intToDouble(int *arr, const size_t n) {
    double *x = (double *)malloc(n * sizeof(double));

    for (int i = 0; i < n; i++) {
        x[i] = (double)arr[i];
    }

    return x;
}

int GetPeakBndIdx(SingleSpectrum *s, double peak_width, int direction) {
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

int checkPeakStd(SingleSpectrum *s, int left_peak_bnd_idx, int n_ch_in_peak) {
    double mean = 0, diff, std = 0;

    for (int i = 0; i < n_ch_in_peak; i++) {
        mean += s->spectrum[left_peak_bnd_idx + i];
    }

    mean /= n_ch_in_peak;

    for (int i = 0; i < n_ch_in_peak; i++) {
        diff = s->spectrum[left_peak_bnd_idx + i] - mean;
        std += diff * diff;
    }

    std = sqrt(std / (n_ch_in_peak - 1));

    if (std < 1) {
        printf(
            "No peak found. Maybe your energy calibration is faulty? "
            "Could not analyze the single spectrum of detector "
            "%s (no S, W & V2P parameters calculated).",
            s->detname
        );
        return 1;
    }

    return 0;
}

// Return values:
// 0: Success
// 1: Failure
int fitPeak(
    double fit_result[5],
    double *x,
    double *y,
    int n_ch_in_peak,
    SingleSpectrum *s
) {
    double max = 0;
    int argmax = 0;

    for (int i = 0; i < n_ch_in_peak; i++) {
        if (y[i] > max) {
            max = y[i],
            argmax = i;
        }
    }

    double x0 = s->energies[argmax];
    double A_erf = y[n_ch_in_peak - 1] - y[0];
    double offset = y[n_ch_in_peak - 1];
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
        x, y, n_ch_in_peak, init, debug
    );

    if (result == NULL) {
        return 1;
    }

    for (int i = 0; i < 5; i++) {
        fit_result[i] = result[i];
    }

    if (debug) {
        puts("####");

        double *fit = (double*)malloc(n_ch_in_peak * sizeof(double));

        if (fit == NULL) {
            free(result);
            return 0;
        }

        for (int i = 0; i < n_ch_in_peak; i++) {
            fit[i] = comb(
                x[i], result[0], result[1], result[2], result[3], result[4]
            );
        }

        double *columns[3] = {x, y, fit};
        char *headers[3] = {"Energy", "Channel Data", "Best Fit"};

        plot(headers, columns, 3, n_ch_in_peak);

        free(fit);
    }

    free(result);
    return 0;
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

    int left_peak_bnd_idx = GetPeakBndIdx(s, peak_width, -1);
    int right_peak_bnd_idx = GetPeakBndIdx(s, peak_width, 1);

    int n_ch_in_peak = right_peak_bnd_idx - left_peak_bnd_idx + 1;

    if (verbose) {
        printf("Extracted peak with %d channels width.\n", n_ch_in_peak);
    }

    if (checkPeakStd(s, left_peak_bnd_idx, n_ch_in_peak) != 0) {
        printf(
            "No peak found. Maybe your energy calibration is faulty? "
            "Could not analyze the single spectrum of detector "
            "%s (no S, W & V2P parameters calculated).",
            s->detname
        );
        return 2;
    }

    double *y = intToDouble(&s->spectrum[left_peak_bnd_idx], n_ch_in_peak);
    double *x = &s->energies[left_peak_bnd_idx];

    double peak_fit_result[5];
    if (fitPeak(peak_fit_result, x, y, n_ch_in_peak, s) != 0) {
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

    if (bg_corr) {
        for (int i = 0; i < n_ch_in_peak; i++) {
            y[i] -= mod_erf(
                x[i],
                peak_fit_result[1],
                peak_fit_result[2],
                peak_fit_result[3],
                peak_fit_result[4]
            );
        }
    }

    double c0;

    switch (follow_peak_order) {
        case 0:
            s->ecal.values[0] -= peak_fit_result[2] - M_POSITRON_keV;
            break;
        case 1:
            c0 = s->ecal.values[0];
            s->ecal.values[1] *= (
                (M_POSITRON_keV - c0) / (peak_fit_result[2] - c0)
            );
            break;
        default:
            break;
    }

    return 0;
}

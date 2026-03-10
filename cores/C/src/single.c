#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "../include/fitting.h"
#include "../include/single.h"
#include "../include/structs.h"

typedef struct {
    int *spectrum;
    double *energies;
    int size;
    int start_idx;
    double *corr_spectrum;
} SpectrumView;

typedef double (*parser_t)(const void*, int);

/*
 * Calculate energies of the channels of SingleSpectrum s with its energy
 * calibration.
 * Return Codes:
 * 0: Success
 * 1: Failure (Missing ecal or malloc fail)
 */
int calcEnergies(SingleSpectrum *s) {
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

static double *intToDouble(const int *arr, size_t n) {
    double *x = (double*)malloc(n * sizeof(double));

    for (size_t i = 0; i < n; i++) {
        x[i] = (double)arr[i];
    }

    return x;
}

static int searchlinear(
    const double *arr, size_t size, const double *ecal, double value
) {
    if (arr[0] > value) return 0;
    if (arr[size-1] < value) return size;
    return (value - ecal[0]) / ecal[1] + 1;
}

static int getPeakBndIdx(
    const SingleSpectrum *s,
    double peak_width,
    int direction
) {
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
static int extractPeak(
    const SingleSpectrum *s,
    SpectrumView *peak,
    double peak_width,
    bool verbose
) {
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

    peak->start_idx = left_peak_bnd_idx;
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
static int checkPeakStd(const SpectrumView *peak, const char *detname) {
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

    if (std < 1) {
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

static int getArgmax(const int *arr, int size) {
    double max = 0;
    int argmax = 0;

    for (int i = 0; i < size; i++) {
        if (arr[i] > max) {
            max = arr[i],
            argmax = i;
        }
    }

    return argmax;
}

static double getDouble(const void *value, int idx) {
    return ((double*)value)[idx];
}

static double getInt(const void *value, int idx) {
    return ((int*)value)[idx];
}

static void dump(
    const char *filename,
    const char *headers[],
    const void *columns[],
    parser_t parsers[],
    int n_cols,
    int n_rows
) {
    if (headers == NULL || columns == NULL || n_cols < 1 || n_rows < 1) return;

    for (int i = 0; i < n_cols; i++) {
        if (headers[i] == NULL || columns[i] == NULL) {
            return;
        }
    }

    FILE *f = fopen(filename, "w");

    if (f == NULL) {
        printf("Could not open file %s\n", filename);
        return;
    }

    for (int i = 0; i < n_cols - 1; i++) {
        fprintf(f, "%s,", headers[i]);
    }
    fprintf(f, "%s\n", headers[n_cols - 1]);

    for (int i = 0; i < n_rows; i++) {
        for (int j = 0; j < n_cols - 1; j++) {
            fprintf(f, "%g,", parsers[j](columns[j], i));
        }
        fprintf(f, "%g\n", parsers[n_cols - 1](columns[n_cols - 1], i));
    }

    fclose(f);
}

static void dumpFitResult(
    double (*func)(double, const double*),
    const double *x,
    const double *y,
    const double *result,
    int size,
    const char *filename
) {
    double *fit = (double*)malloc(size * sizeof(double));

    if (fit == NULL) {
        return;
    }

    for (int i = 0; i < size; i++) {
        fit[i] = func(x[i], result);
    }

    const void *columns[3] = {x, y, fit};
    const char *headers[3] = {"Energy", "Channel Data", "Best Fit"};
    parser_t parsers[3] = {getDouble, getDouble, getDouble};

    dump(filename, headers, columns, parsers, 3, size);

    free(fit);
}

static double gaussDebug(double x, const double *result) {
    return gaussian(x, result[0], result[1], result[2]);
}

// Return values:
// 0: Success
// 1: Failure
static int fitPeakGauss(double fit_result[3], SpectrumView *peak, bool debug) {
    int argmax = getArgmax(peak->spectrum, peak->size);
    double max = peak->corr_spectrum[argmax];
    double x0 = peak->energies[argmax];

    double init[3] = {max, x0, 1.5};

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
        dumpFitResult(
            gaussDebug, peak->energies, peak->corr_spectrum, result, peak->size,
            "debug_Gaussian_fit.csv"
        );
    }

    free(result);
    return 0;
}

// Return values:
// 0: Success
// 1: Failure
static int correctEcal(
    SingleSpectrum *s,
    SpectrumView *peak,
    int follow_peak_order,
    bool verbose,
    bool debug
) {
    double peak_fit_result[3];

    int fit_status = fitPeakGauss(peak_fit_result, peak, debug);

    if (fit_status != 0) {
        printf("Gaussian Fit of spectrum failed\n");
        return 1;
    }

    double c0 = s->ecal.values[0], c1 = s->ecal.values[1];

    if (follow_peak_order == 0) {
        s->ecal.values[0] -= peak_fit_result[1] - M_POSITRON_keV;
        if (verbose) {
            printf(
                "Corrected 0th order ecal coefficient: %f -> %f\n",
                c0, s->ecal.values[0]
            );
        }
    } else {
        s->ecal.values[1] *= (
            (M_POSITRON_keV - c0) / (peak_fit_result[1] - c0)
        );
        if (verbose) {
            printf(
                "Corrected 1st order ecal coefficient: %f -> %f\n",
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

static double combDebug(double x, const double *result) {
    return comb(x, result[0], result[1], result[2], result[3], result[4]);
}

// Return values:
// 0: Success
// 1: Failure
static int fitPeakComb(double fit_result[5], SpectrumView *peak, bool debug) {
    int argmax = getArgmax(peak->spectrum, peak->size);
    double max = peak->corr_spectrum[argmax];
    double x0 = peak->energies[argmax];
    double A_erf = peak->corr_spectrum[peak->size - 1] - peak->corr_spectrum[0];
    double offset = peak->corr_spectrum[peak->size - 1];
    double A_gauss = max - offset - A_erf / 2;

    double init[5] = {A_gauss, A_erf, x0, 1.5, offset};

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
        dumpFitResult(
            combDebug, peak->energies, peak->corr_spectrum, result, peak->size,
            "debug_Gaussian_Erf_fit.csv"
        );
    }

    free(result);
    return 0;
}

static double sumArr(
    const void *spectrum, int start_idx, int end_idx, parser_t parser
) {
    double sum = 0;

    for (int i = start_idx; i < end_idx; i++) {
        sum += parser(spectrum, i);
    }

    return sum;
}

static int correctPeak(SpectrumView *peak, bool verbose, bool debug) {
    double peak_fit_result[5];
    if (fitPeakComb(peak_fit_result, peak, debug) != 0) {
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

    if (verbose) {
        double counts_before, counts_after;
        counts_before = sumArr(peak->spectrum, 0, peak->size, getInt);
        counts_after = sumArr(peak->corr_spectrum, 0, peak->size, getDouble);

        printf(
            "Performed background subtraction: subtracted fraction: %f%%\n",
            100 * (counts_before - counts_after) / counts_before
        );
    }

    if (debug) {
        const void *columns[3] = {
            peak->energies, peak->spectrum, peak->corr_spectrum
        };
        const char *headers[3] = {
            "Energy", "Original Counts", "Corrected Counts"
        };
        parser_t parsers[3] = {getDouble, getInt, getDouble};
        dump("corrected_peak.csv", headers, columns, parsers, 3, peak->size);
    }

    return 0;
}

static double integrateSpectrum(
    const void *spectrum,
    const double *energies,
    size_t size,
    int offset,
    parser_t parser,
    const double *ecal,
    double left_e,
    double right_e
) {
    double counts = 0;

    int left_inner_idx, right_inner_idx;

    left_inner_idx = searchlinear(energies, size, ecal, left_e) + 1 - offset;
    right_inner_idx = searchlinear(energies, size, ecal, right_e) - 1 - offset;

    printf("Integrating from %d to %d\n", left_inner_idx, right_inner_idx);

    counts += sumArr(spectrum, left_inner_idx, right_inner_idx, parser);

    counts += parser(spectrum, left_inner_idx - 1) * (
        fmod(left_e - ecal[0], ecal[1]) / ecal[1]
    );

    counts += parser(spectrum, right_inner_idx + 1) * (
        fmod(right_e - ecal[0], ecal[1]) / ecal[1]
    );

    return counts;
}

static void calcLineshapeParams(
    SingleSpectrum *s,
    const SpectrumView *peak,
    double s_width,
    double w_width,
    double w_dist,
    bool w_rightonly,
    const double v2p_bounds[4]
) {
    s->counts = sumArr(s->spectrum, 0, s->spectrum_size, getInt);
    s->dcounts = sqrt(s->counts);

    s->peak_counts = sumArr(peak->corr_spectrum, 0, peak->size, getDouble);
    s->dpeak_counts = sqrt(s->peak_counts);

    double s_area, w_area, w_l_area, w_r_area, valley_area, v_peak_area;

    s_area = integrateSpectrum(
        peak->corr_spectrum, peak->energies, peak->size,
        peak->start_idx, getDouble, s->ecal.values,
        M_POSITRON_keV - s_width, M_POSITRON_keV + s_width
    );

    w_r_area = integrateSpectrum(
        peak->corr_spectrum, peak->energies, peak->size,
        peak->start_idx, getDouble, s->ecal.values,
        M_POSITRON_keV + w_dist, M_POSITRON_keV + w_width + w_dist
    );
    w_l_area = w_rightonly ? 0 : integrateSpectrum(
        peak->corr_spectrum, peak->energies, peak->size,
        peak->start_idx, getDouble, s->ecal.values,
        M_POSITRON_keV - w_width - w_dist, M_POSITRON_keV - w_dist
    );
    w_area = w_r_area + w_l_area;

    valley_area = integrateSpectrum(
        s->spectrum, s->energies, s->spectrum_size, 0, getInt, s->ecal.values,
        v2p_bounds[0], v2p_bounds[1]
    );
    v_peak_area = integrateSpectrum(
        s->spectrum, s->energies, s->spectrum_size, 0, getInt, s->ecal.values,
        v2p_bounds[2], v2p_bounds[3]
    );

    s->s = s_area / s->peak_counts;
    s->w = w_area / s->peak_counts;
    s->v2p = valley_area / v_peak_area;

    s->ds = sqrt(s->s * (1 - s->s) / s->peak_counts);
    s->dw = sqrt(s->w * (1 - s->w) / s->peak_counts);
    s->dv2p = s->v2p * sqrt(1 / valley_area + 1 / v_peak_area);
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
    const double v2p_bounds[4],
    const unsigned int follow_peak_order,
    const bool verbose,
    const bool debug
) {
    if (!singleComplete(s)) return 1;

    if (verbose) {
        printf("Analyzing Single Spectrum of detector %s...\n", s->detname);
    }

    SpectrumView peak;

    peak.corr_spectrum = (double*)malloc(s->spectrum_size * sizeof(double));
    if (peak.corr_spectrum == NULL) {
        return 3;
    }

    if (extractPeak(s, &peak, peak_width, verbose) != 0) {
        free(peak.corr_spectrum);
        return 1;
    }

    if (checkPeakStd(&peak, s->detname) != 0) {
        free(peak.corr_spectrum);
        return 2;
    }

    if (follow_peak_order < 2 && !s->ecal_corrected) {
        if (correctEcal(s, &peak, follow_peak_order, verbose, debug) != 0) {
            free(peak.corr_spectrum);
            return 1;
        }
        if (extractPeak(s, &peak, peak_width, verbose) != 0) {
            free(peak.corr_spectrum);
            return 1;
        }
    }

    if (bg_corr && correctPeak(&peak, verbose, debug) != 0) {
        free(peak.corr_spectrum);
        return 1;
    }

    calcLineshapeParams(
        s, &peak, s_width, w_width, w_dist, w_rightonly, v2p_bounds
    );

    if (peak.corr_spectrum != NULL) free(peak.corr_spectrum);

    return 0;
}

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

/*
 * Plot data with Python.
 */
void plot(
    char *headers[], double *columns[], const int n_cols, const int n_rows
) {
    if (n_cols < 1 || n_rows < 1) return;

    FILE *f = fopen("/tmp/stacs_plot_data.csv", "w");

    if (f == NULL) {
        puts("Export of plot data to /tmp/stacs_plot_data.csv failed.");
        return;
    }

    if (headers != NULL) {
        if (n_cols > 1) {
            for (int i = 0; i < n_cols - 1; i++) {
                fprintf(f, "%s,", headers[i]);
            }
        }
        fprintf(f, "%s\n", headers[n_cols - 1]);
    } else {
        if (n_cols > 1) {
            for (int i = 0; i < n_cols - 1; i++)
                fprintf(f, "%d,", i);
        }
        fprintf(f, "%d\n", n_cols - 1);
    }

    for (int i = 0; i < n_rows; i++) {
        if (n_cols > 1) {
            for (int j = 0; j < n_cols - 1; j++)
                fprintf(f, "%g,", columns[j][i]);
        }
        fprintf(f, "%g\n", columns[n_cols - 1][i]);
    }

    fclose(f);

    system("python3 ./plot.py");
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

    for (int i = 0; i < n; i++)
        x[i] = (double)arr[i];

    return x;
}

void plotSingle(SingleSpectrum *s) {
    if (!singleComplete(s)) return;

    int n = s->spectrum_size;

    double *spectrum = intToDouble(s->spectrum, n);

    char *headers[2] = {"Energy", "Counts"};
    double *columns[2] = {s->energies, spectrum};

    plot(headers, columns, 2, n);

    free(spectrum);
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
    if (!singleComplete(s)) {
        return 1;
    }

    if (verbose) {
        printf("Analyzing Single Spectrum of detector %s...\n", s->detname);
    }

    double left_peak_bnd_x, right_peak_bnd_x;
    int left_peak_bnd_idx, right_peak_bnd_idx;

    left_peak_bnd_x = ((M_POSITRON_keV - peak_width / 2) - s->ecal.values[0]) /
                      s->ecal.values[1];

    right_peak_bnd_x = ((M_POSITRON_keV + peak_width / 2) - s->ecal.values[0]) /
                       s->ecal.values[1];

    if (left_peak_bnd_x < 0) {
        left_peak_bnd_idx = 0;
    } else if (left_peak_bnd_x > s->spectrum_size - 1) {
        left_peak_bnd_idx = s->spectrum_size - 1;
    } else {
        left_peak_bnd_idx = round(left_peak_bnd_x);
    }

    if (right_peak_bnd_x < 0) {
        right_peak_bnd_idx = 0;
    } else if (right_peak_bnd_x > s->spectrum_size - 1) {
        right_peak_bnd_idx = s->spectrum_size - 1;
    } else {
        right_peak_bnd_idx = round(right_peak_bnd_x);
    }

    int n_ch_in_peak = right_peak_bnd_idx - left_peak_bnd_idx + 1;

    if (verbose) {
        printf("Extracted peak with %d channels width.\n", n_ch_in_peak);
    }

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
        return 2;
    }

    double max = 0; // init guess for gaussian amplitude
    int argmax = 0;

    for (int i = 0; i < n_ch_in_peak; i++) {
        if (s->spectrum[left_peak_bnd_idx + i] > max) {
            max = s->spectrum[left_peak_bnd_idx + i],
            argmax = left_peak_bnd_idx + i;
        }
    }

    double x0 = s->energies[argmax];
    double init[3] = {max, x0, (double)1.5};
    double *y = (double *)malloc(n_ch_in_peak * sizeof(double));

    if (y == NULL) {
        puts("Could not convert int array to double\n");
        return 3;
    }

    for (int i = 0; i < n_ch_in_peak; i++) {
        y[i] = (double)s->spectrum[left_peak_bnd_idx + i];
    }

    if (bg_corr == false) {
        if (debug) puts("####\nResults of peak fit:");

        // contains amp, pos and sig of peak
        double *result = fitGaussian(
            &(s->energies[left_peak_bnd_idx]), y, n_ch_in_peak, init, debug
        );

        if (debug) {
            puts("####");

            double *fit = (double *)malloc(n_ch_in_peak * sizeof(double));

            for (int i = 0; i < n_ch_in_peak; i++) {
                fit[i] = gaussian(
                    s->energies[left_peak_bnd_idx + i], result[0], result[1],
                    result[2]
                );
            }

            double *y =
                intToDouble(&(s->spectrum[left_peak_bnd_idx]), n_ch_in_peak);

            double *columns[3] = {&(s->energies[left_peak_bnd_idx]), y, fit};

            char *headers[3] = {"Energy", "Channel Data", "Best Fit"};

            plot(headers, columns, 3, n_ch_in_peak);

            free(y);
            free(fit);
        }

        if (verbose) {
            printf(
                "Peak center %.3f keV deviates from %.3f "
                "keV by %.3f keV (< 0.5 keV is typically okay).\n",
                result[1], M_POSITRON_keV, result[1] - M_POSITRON_keV
            );
        }

        free(result);
    } else {
        double bg_width = bg_frac * peak_width / 2;
    }

    return 0;
}

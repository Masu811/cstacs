#ifndef SINGLE_H
#define SINGLE_H

#include <gsl/gsl_multifit_nlinear.h>

#define M_POSITRON_keV 511

typedef struct {
        size_t n;
        double *x;
        double *y;
} data_t;

static void
calcEnergies(SingleSpectrum *s);

void
exportTable(const double *columns[], const int n_cols, const int n_rows,
            const char *filepath);

static int
gaussian_f(const gsl_vector *params, void *data, gsl_vector *residuals);

static int
gaussian_df(const gsl_vector *params, void *data, gsl_matrix *J);

static void
callback(const size_t iter, void *params,
         const gsl_multifit_nlinear_workspace *w);

static double
*fitGaussian(double *x, double *y, const int N, double init[3],
             const int verbose);

int
analyze(SingleSpectrum *s, const double s_width, const double w_width,
        const double w_dist, const int w_rightonly, const double peak_width,
        const double bg_frac, const int bg_corr, const double v2p_bounds[4],
        const int follow_peak_order, int verbose, int debug);

#endif
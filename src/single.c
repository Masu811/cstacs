#include <stdio.h>
#include <stdlib.h>
#include <math.h>
//#include <gsl/gsl_multifit_nlin.h>
#include <gsl/gsl_multifit_nlinear.h>
#include <gsl/gsl_blas.h>

#include "structs.h"
#include "single.h"

#define M_POSITRON_keV 511

/*
 * Calculate energies of the channels of SingleSpectrum s with its energy
 * calibration.
 */
static void
calcEnergies(SingleSpectrum *s)
{
    if (s->ecal.ref == NULL) {
        puts("Single Spectrum is missing energy calibration");
        return;
    }

    double *energies = (double*)malloc(s->spectrum_size * sizeof(double));

    if (energies == NULL) {
        puts("Memory allocation for energy array failed");
        return;
    }

    for (int i = 0; i < s->spectrum_size; i++) {
        energies[i] = (s->ecal.values[0] + i * s->ecal.values[1]);
    }

    s->energies = energies;
}

/*
 * Save energies and counts columnwise and tab separated in a txt file.
 */
void
exportTable(const double *columns[], const int n_cols, const int n_rows,
            const char *filepath)
{
    FILE *f = fopen(filepath, "w");

    if (f == NULL) {
        printf("Could not open/create file %s\n. Path might be invalid\n",
               filepath);
        return;
    }
    
    for (int i = 0; i < n_rows; i++) {
        if (n_cols > 1) {
            for (int j = 0; j < n_cols - 1; j++) {
                fprintf(f, "%g\t", columns[j][i]);
            }
        }
        fprintf(f, "%g\n", columns[n_cols - 1][i]);
    }
    
    fclose(f);
}

static int
gaussian_f(const gsl_vector *params, void *data, gsl_vector *f)
{
    size_t n = ((data_t*)data)->n;
    double *x = ((data_t*)data)->x;
    double *y = ((data_t*)data)->y;

    double A = gsl_vector_get(params, 0);
    double x0 = gsl_vector_get(params, 1);
    double log_sigma = gsl_vector_get(params, 2);
    double sigma = exp(log_sigma);

    for (int i = 0; i < n; i++) {
        double Yi = A * exp(-0.5 * pow((x[i] - x0) / sigma, 2));
        gsl_vector_set(f, i, Yi - y[i]);
    }

    return GSL_SUCCESS;
}

static int
gaussian_df(const gsl_vector *params, void *data, gsl_matrix *J)
{
    size_t n = ((data_t*)data)->n;
    double *x = ((data_t*)data)->x;

    double A = gsl_vector_get(params, 0);
    double x0 = gsl_vector_get(params, 1);
    double log_sigma = gsl_vector_get(params, 2);
    double sigma = exp(log_sigma); // Ensure sigma is positive

    for (size_t i = 0; i < n; i++) {
        double df_dA = exp(-0.5 * pow((x[i] - x0) / sigma, 2));
        double df_dx0 = A * df_dA * (x[i] - x0) / (sigma * sigma);
        double df_dlog_sigma = df_dx0 * (x[i] - x0);

        gsl_matrix_set(J, i, 0, df_dA);
        gsl_matrix_set(J, i, 1, df_dx0);
        gsl_matrix_set(J, i, 2, df_dlog_sigma);
    }

    return GSL_SUCCESS;
}

static void
gaussian_callback(const size_t iter, void *params,
                  const gsl_multifit_nlinear_workspace *w)
{
    gsl_vector *f = gsl_multifit_nlinear_residual(w);
    gsl_vector *x = gsl_multifit_nlinear_position(w);
    double rcond;

    /* compute reciprocal condition number of J(x) */
    gsl_multifit_nlinear_rcond(&rcond, w);
    /*
    fprintf(stderr, "iter %2zu: A = %.4f, x0 = %.4f, sigma = %.4f, cond(J) = %8.4f, |f(x)| = %.4f\n",
            iter,
            gsl_vector_get(x, 0),
            gsl_vector_get(x, 1),
            gsl_vector_get(x, 2),
            1.0 / rcond,
            gsl_blas_dnrm2(f));
    */
}

static double
*fitGaussian(double *x, double *y, const int N, double init[3], const int verbose)
{
    const gsl_multifit_nlinear_type *T = gsl_multifit_nlinear_trust;
    gsl_multifit_nlinear_workspace *w;
    gsl_multifit_nlinear_fdf fdf;
    gsl_multifit_nlinear_parameters fdf_params =
        gsl_multifit_nlinear_default_parameters();
    const size_t n = N;
    const size_t p = 3;

    gsl_vector *f;
    gsl_matrix *J;
    gsl_matrix *covar = gsl_matrix_alloc(p, p);
    data_t d = {N, x, y};
    gsl_vector_view params = gsl_vector_view_array(init, p);
    double chisq, chisq0;
    int status, info;
    size_t i;

    const double xtol = 1e-8;
    const double gtol = 1e-8;
    const double ftol = 0.0;

    /* define the function to be minimized */
    fdf.f = gaussian_f;
    fdf.df = gaussian_df;   /* set to NULL for finite-difference Jacobian */
    fdf.fvv = NULL;     /* not using geodesic acceleration */
    fdf.n = n;
    fdf.p = p;
    fdf.params = &d;

    /* allocate workspace with default parameters */
    w = gsl_multifit_nlinear_alloc(T, &fdf_params, n, p);

    /* initialize solver with starting point and weights */
    gsl_multifit_nlinear_init(&params.vector, &fdf, w);

    /* compute initial cost function */
    f = gsl_multifit_nlinear_residual(w);
    gsl_blas_ddot(f, f, &chisq0);

    /* solve the system with a maximum of 100 iterations */
    status = gsl_multifit_nlinear_driver(100, xtol, gtol, ftol,
                                         gaussian_callback, NULL, &info, w);

    /* compute covariance of best fit parameters */
    J = gsl_multifit_nlinear_jac(w);
    gsl_multifit_nlinear_covar(J, 0.0, covar);

    /* compute final cost */
    gsl_blas_ddot(f, f, &chisq);

    if (verbose) {
        #define FIT(i) gsl_vector_get(w->x, i)
        #define ERR(i) sqrt(gsl_matrix_get(covar,i,i))

        fprintf(stderr, "summary from method '%s/%s'\n",
                gsl_multifit_nlinear_name(w),
                gsl_multifit_nlinear_trs_name(w));
        fprintf(stderr, "number of iterations: %zu\n",
                gsl_multifit_nlinear_niter(w));
        fprintf(stderr, "function evaluations: %zu\n", fdf.nevalf);
        fprintf(stderr, "Jacobian evaluations: %zu\n", fdf.nevaldf);
        fprintf(stderr, "reason for stopping: %s\n",
                (info == 1) ? "small step size" : "small gradient");
        fprintf(stderr, "initial |f(x)| = %f\n", sqrt(chisq0));
        fprintf(stderr, "final   |f(x)| = %f\n", sqrt(chisq));

        {
            double dof = n - p;
            double c = GSL_MAX_DBL(1, sqrt(chisq / dof));

            fprintf(stderr, "chisq/dof = %g\n", chisq / dof);

            fprintf (stderr, "A      = %.5f +/- %.5f\n", FIT(0), c*ERR(0));
            fprintf (stderr, "x0     = %.5f +/- %.5f\n", FIT(1), c*ERR(1));
            fprintf (stderr, "sigma  = %.5f +/- %.5f\n", FIT(2), c*ERR(2));
        }

        fprintf (stderr, "status = %s\n", gsl_strerror(status));
    }

    double *result = (double*)malloc(3 * sizeof(double));

    result[0] = gsl_vector_get(w->x, 0);
    result[1] = gsl_vector_get(w->x, 1);
    result[2] = gsl_vector_get(w->x, 2);

    gsl_multifit_nlinear_free(w);
    gsl_matrix_free(covar);

    return result;
}

/*
 * Calculate line shape parameters for SingleSpectrum.
 * Return codes:
 * 0: Success
 * 1: SingleSpectrum is missing some data for some calculations.
 * 2: No peak found
 * 3: Memory allocation failed
 */
int
analyze(SingleSpectrum *s, const double s_width, const double w_width,
        const double w_dist, const int w_rightonly, const double peak_width,
        const double bg_frac, const int bg_corr, const double v2p_bounds[4],
        const int follow_peak_order, int verbose, int debug)
{
    if (s == NULL) {
        puts("Cannot analyze uninitialized SingleSpectrum");
        return 1;
    }
    if (s->spectrum == NULL) {
        puts("Cannot analyze empty SingleSpectrum");
        return 1;
    }
    if (s->ecal.values == 0 && s->energies == NULL) {
        puts("Cannot analyze SingleSpectrum without energy calibration");
        return 1;
    }

    if (verbose) {
        printf("Analyzing single spectrum of detector %s...\n",
               s->detname.name);
    }

    if (s->energies == NULL) {
        calcEnergies(s);
    }

    double left_peak_bnd_x, right_peak_bnd_x;
    int left_peak_bnd_idx, right_peak_bnd_idx;

    left_peak_bnd_x = ((M_POSITRON_keV - peak_width / 2) - s->ecal.values[0]) \
                      / s->ecal.values[1];
    right_peak_bnd_x = ((M_POSITRON_keV + peak_width / 2) - s->ecal.values[0]) \
                       / s->ecal.values[1];

    if (left_peak_bnd_x < 0) {
        left_peak_bnd_idx = 0;
    } else if (left_peak_bnd_x > s->spectrum_size) {
        left_peak_bnd_idx = s->spectrum_size - 1;
    } else {
        left_peak_bnd_idx = round(left_peak_bnd_x);
    }

    if (right_peak_bnd_x < 0) {
        right_peak_bnd_idx = 0;
    } else if (right_peak_bnd_x > s->spectrum_size) {
        right_peak_bnd_idx = s->spectrum_size - 1;
    } else {
        right_peak_bnd_idx = round(right_peak_bnd_x);
    }

    int n_ch_in_peak = right_peak_bnd_idx - left_peak_bnd_idx + 1;

    if (verbose) {
        printf("Extracted peak with %d channels width.\n", n_ch_in_peak);
    }

    double mean = 0, std = 0;

    for (int i = 0; i < n_ch_in_peak; i++) {
        mean += s->spectrum[left_peak_bnd_idx + i];
    }
    mean /= n_ch_in_peak;
    for (int i = 0; i < n_ch_in_peak; i++) {
        double diff = s->spectrum[left_peak_bnd_idx + i] - mean;
        std += diff * diff;
    }
    std = sqrt(std / (n_ch_in_peak - 1));

    if (std < 1) {
        printf("No peak found. Maybe your energy calibration is faulty? "
               "Could not analyze the single spectrum of detector "
               "%s (no S, W & V2P parameters calculated).", s->detname);
        return 2;
    }

    double max = 0;  // init guess for gaussian amplitude
    int argmax = 0;

    for (int i = 0; i < n_ch_in_peak; i++) {
        if (s->spectrum[left_peak_bnd_idx + i] > max) {
            max = s->spectrum[left_peak_bnd_idx + i],
            argmax = left_peak_bnd_idx;
        }
    }

    double x0 = s->energies[argmax];

    double init[3] = {max, x0, (double)1.5};
    double *y = (double*)malloc(n_ch_in_peak * sizeof(double));

    if (y == NULL) {
        puts("Could not convert int array to double\n");
        return 3;
    }

    for (int i = 0; i < n_ch_in_peak; i++) {
        y[i] = (double)s->spectrum[left_peak_bnd_idx + i];
    }

    double *result = fitGaussian(&(s->energies[left_peak_bnd_idx]),
                                 y,
                                 n_ch_in_peak,
                                 init,
                                 debug);

    free(result);

    return 0;
}

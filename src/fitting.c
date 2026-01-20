/*
 * Fitting routines taken from
 * https://www.gnu.org/software/gsl/doc/html/nls.html
 */

#include <gsl/gsl_blas.h>
#include <gsl/gsl_multifit_nlinear.h>
#include <gsl/gsl_vector_double.h>
#include <stdio.h>

#include "../include/fitting.h"

const double xtol = 1e-6;
const double gtol = 1e-6;
const double ftol = 0.0;

////////// Fit Utilities //////////

/*
 * Print summary of the performed fit.
 * Requires the fit to have run and covariance matrix to have been calculated.
 */
static void printFitReport(FitEnv *fit_env) {
    fprintf(
        stderr,
        "summary from method '%s/%s'\n",
        gsl_multifit_nlinear_name(fit_env->w),
        gsl_multifit_nlinear_trs_name(fit_env->w)
    );
    fprintf(
        stderr,
        "number of iterations: %zu\n",
        gsl_multifit_nlinear_niter(fit_env->w)
    );
    fprintf(stderr, "function evaluations: %zu\n", fit_env->fdf.nevalf);
    fprintf(stderr, "Jacobian evaluations: %zu\n", fit_env->fdf.nevaldf);
    fprintf(
        stderr, "reason for stopping: %s\n",
        (fit_env->info == 1) ? "small step size" : "small gradient"
    );
    fprintf(stderr, "initial |f(x)| = %f\n", sqrt(fit_env->chisq0));
    fprintf(stderr, "final   |f(x)| = %f\n", sqrt(fit_env->chisq));

    double dof = fit_env->fdf.n - fit_env->fdf.p;
    double c = GSL_MAX_DBL(1, sqrt(fit_env->chisq / dof));

    fprintf(stderr, "chisq/dof = %g\n", fit_env->chisq / dof);

    for (int i = 0; i < fit_env->fdf.p; i++) {
        fprintf(
            stderr,
            "x[%d] = %.5f +/- %.5f\n",
            i,
            gsl_vector_get(fit_env->w->x, i),
            c * sqrt(gsl_matrix_get(fit_env->covar, i, i))
        );
    }

    fprintf(stderr, "status = %s\n", gsl_strerror(fit_env->status));
}

/*
 * Prepare the fit environment for starting the fit.
 *
 * This function allocates memory that needs to be freed by the caller:
 * `fit_env->w`
 *
 * Return values:
 * 0: Success
 * 1: Failure (Malloc fail)
 */
static int prepareFit(
    FitEnv *fit_env,
    double *x,
    double *y,
    size_t n,
    size_t p,
    double *init,
    int (*f)(const gsl_vector*, void*, gsl_vector*),
    int (*df)(const gsl_vector* , void*, gsl_matrix*)
) {
    /* define the function to be minimized */
    fit_env->fdf.f = f;
    fit_env->fdf.df = df;    /* set to NULL for finite-difference Jacobian */
    fit_env->fdf.fvv = NULL; /* not using geodesic acceleration */
    fit_env->fdf.n = n;
    fit_env->fdf.p = p;
    fit_env->fdf.params = &fit_env->d;
    fit_env->d.n = n;
    fit_env->d.x = x;
    fit_env->d.y = y;

    const gsl_multifit_nlinear_type *T = gsl_multifit_nlinear_trust;
    gsl_multifit_nlinear_parameters fdf_params = (
        gsl_multifit_nlinear_default_parameters()
    );
    gsl_vector_view params = gsl_vector_view_array(init, fit_env->fdf.p);

    /* allocate workspace with default parameters */
    fit_env->w = gsl_multifit_nlinear_alloc(T, &fdf_params, n, p);

    if (fit_env->w == NULL) {
        fprintf(stderr, "Could not allocate memory for workspace\n");
        return 1;
    }

    /* initialize solver with starting point and weights */
    gsl_multifit_nlinear_init(&params.vector, &fit_env->fdf, fit_env->w);

    /* compute initial cost function */
    gsl_vector *res = gsl_multifit_nlinear_residual(fit_env->w);
    gsl_blas_ddot(res, res, &fit_env->chisq0);

    return 0;
}

/*
 * Calculate the covariance matrix
 *
 * This function allocates memory that needs to be freed by the caller:
 * `fit_env->covar`
 *
 * Return values:
 * 0: Success
 * 1: Failure (Malloc fail)
 */
static int calcCovar(FitEnv *fit_env) {
    /* compute covariance of best fit parameters */
    gsl_matrix *J = gsl_multifit_nlinear_jac(fit_env->w);
    fit_env->covar = gsl_matrix_alloc(fit_env->fdf.p, fit_env->fdf.p);

    if (fit_env->covar == NULL) {
        fprintf(stderr, "Could not allocate memory for covariance matrix\n");
        return 1;
    }

    gsl_multifit_nlinear_covar(J, 0.0, fit_env->covar);

    /* compute final cost */
    gsl_vector *res = gsl_multifit_nlinear_residual(fit_env->w);
    gsl_blas_ddot(res, res, &fit_env->chisq);

    return 0;
}

/*
 * Create result array and optionally print fit report
 *
 * The returned array needs to be freed by the caller.
 *
 * Returns NULL in case of error
 */
static double *postFit(FitEnv *fit_env, int debug) {
    if (debug && calcCovar(fit_env) == 0) {
        printFitReport(fit_env);
        gsl_matrix_free(fit_env->covar);
    }

    double *result = (double*)malloc(fit_env->fdf.p * sizeof(double));

    if (result == NULL) {
        fprintf(stderr, "Could not allocate memory for result\n");
        return NULL;
    }

    gsl_vector *x = gsl_multifit_nlinear_position(fit_env->w);

    for (int i = 0; i < fit_env->fdf.p; i++) {
        result[i] = gsl_vector_get(x, i);
    }

    gsl_multifit_nlinear_free(fit_env->w);

    return result;
}

/*
 * Perform a fit of the given function to the given data
 *
 * The returned array needs to be freed by the caller.
 *
 * Returns NULL in case of error
 */
static double *fit(
    double *x,
    double *y,
    size_t n,
    size_t p,
    double *init,
    int (*f)(const gsl_vector*, void*, gsl_vector*),
    int (*df)(const gsl_vector* , void*, gsl_matrix*),
    void (*callback)(
        const size_t, void*, const gsl_multifit_nlinear_workspace*
    ),
    int debug
) {
    FitEnv fit_env;

    if (prepareFit(&fit_env, x, y, n, p, init, f, df) != 0) {
        return NULL;
    }

    if (!debug) callback = NULL;

    /* solve the system with a maximum of 100 iterations */
    fit_env.status = gsl_multifit_nlinear_driver(
        100, xtol, gtol, ftol, callback, NULL, &fit_env.info, fit_env.w
    );

    return postFit(&fit_env, debug);
}

////////// 1D Gaussian //////////

double gaussian(double x, double A, double x0, double sigma) {
    double z = (x - x0) / sigma;
    return A * exp(-0.5 * z * z);
}

static int gaussian_f(const gsl_vector *params, void *data, gsl_vector *f) {
    size_t n = ((data_t*)data)->n;
    double *x = ((data_t*)data)->x;
    double *y = ((data_t*)data)->y;

    double A = gsl_vector_get(params, 0);
    double x0 = gsl_vector_get(params, 1);
    double sigma = gsl_vector_get(params, 2);

    if (fabs(sigma) < 1e-10) return GSL_FAILURE;

    for (int i = 0; i < n; i++) {
        double Yi = gaussian(x[i], A, x0, sigma);
        gsl_vector_set(f, i, Yi - y[i]);
    }

    return GSL_SUCCESS;
}

static int gaussian_df(const gsl_vector *params, void *data, gsl_matrix *J) {
    size_t n = ((data_t*)data)->n;
    double *x = ((data_t*)data)->x;

    double A = gsl_vector_get(params, 0);
    double x0 = gsl_vector_get(params, 1);
    double sigma = gsl_vector_get(params, 2);

    if (fabs(sigma) < 1e-10) return GSL_FAILURE;

    for (size_t i = 0; i < n; i++) {
        double xi = x[i];
        double zi = (xi - x0) / sigma;
        double ei = exp(-0.5 * zi * zi);

        double df_dA = ei;
        double df_dx0 = (A / sigma) * ei * zi;
        double df_dsigma = (A / sigma) * ei * zi * zi;

        gsl_matrix_set(J, i, 0, df_dA);
        gsl_matrix_set(J, i, 1, df_dx0);
        gsl_matrix_set(J, i, 2, df_dsigma);
    }

    return GSL_SUCCESS;
}

static void gaussian_callback(
    const size_t iter, void *params, const gsl_multifit_nlinear_workspace *w
) {
    gsl_vector *f = gsl_multifit_nlinear_residual(w);
    gsl_vector *x = gsl_multifit_nlinear_position(w);
    double rcond;

    /* compute reciprocal condition number of J(x) */
    gsl_multifit_nlinear_rcond(&rcond, w);

    printf(
        "iter %2zu: A = %.4f, x0 = %.4f, sigma = %.4f, cond(J) = %8.4f, "
        "|f(x)| = %.4f\n",
        iter, gsl_vector_get(x, 0), gsl_vector_get(x, 1),
        gsl_vector_get(x, 2), 1.0 / rcond, gsl_blas_dnrm2(f)
    );
}

double *fitGaussian(
    double *x, double *y, size_t N, double init[3], int debug
) {
    return fit(
        x, y, N, 3, init, &gaussian_f, &gaussian_df, &gaussian_callback, debug
    );
}

////////// Erf //////////

double mod_erf(double x, double A, double x0, double sigma, double offset) {
    return A * erf((x - x0) / sigma) + offset;
}

static int erf_f(const gsl_vector *params, void *data, gsl_vector *f) {
    size_t n = ((data_t*)data)->n;
    double *x = ((data_t*)data)->x;
    double *y = ((data_t*)data)->y;

    double A = gsl_vector_get(params, 0);
    double x0 = gsl_vector_get(params, 1);
    double sigma = gsl_vector_get(params, 2);
    double offset = gsl_vector_get(params, 3);

    if (sigma < 1e-10) return GSL_FAILURE;

    for (int i = 0; i < n; i++) {
        double Yi = mod_erf(x[i], A, x0, sigma, offset);
        gsl_vector_set(f, i, Yi - y[i]);
    }

    return GSL_SUCCESS;
}

static int erf_df(const gsl_vector *params, void *data, gsl_matrix *J) {
    size_t n = ((data_t*)data)->n;
    double *x = ((data_t*)data)->x;

    double A = gsl_vector_get(params, 0);
    double x0 = gsl_vector_get(params, 1);
    double sigma = gsl_vector_get(params, 2);

    if (sigma < 1e-10) return GSL_FAILURE;

    for (size_t i = 0; i < n; i++) {
        double xi = x[i];
        double zi = (xi - x0) / sigma;
        double ei = exp(-zi * zi);
        double erfi = erf(zi);
        double c = 2.0 / sqrt(M_PI);

        double df_dA = erfi;
        double df_dx0 = -c * A * ei / sigma;
        double df_dsigma = -c * A * ei * zi / sigma;
        double df_doffset = 1;

        gsl_matrix_set(J, i, 0, df_dA);
        gsl_matrix_set(J, i, 1, df_dx0);
        gsl_matrix_set(J, i, 2, df_dsigma);
        gsl_matrix_set(J, i, 3, df_doffset);
    }

    return GSL_SUCCESS;
}

static void erf_callback(
    const size_t iter, void *params, const gsl_multifit_nlinear_workspace *w
) {
    gsl_vector *f = gsl_multifit_nlinear_residual(w);
    gsl_vector *x = gsl_multifit_nlinear_position(w);
    double rcond;

    /* compute reciprocal condition number of J(x) */
    gsl_multifit_nlinear_rcond(&rcond, w);

    printf(
        "iter %2zu: A = %.4f, x0 = %.4f, sigma = %.4f, offset = %.4f, "
        "cond(J) = %8.4f, |f(x)| = %.4f\n",
        iter, gsl_vector_get(x, 0), gsl_vector_get(x, 1),
        gsl_vector_get(x, 2), gsl_vector_get(x, 3),
        1.0 / rcond, gsl_blas_dnrm2(f)
    );
}

double *fitErf(double *x, double *y, size_t N, double init[4], int debug) {
    return fit(x, y, N, 4, init, &erf_f, &erf_df, &erf_callback, debug);
}

////////// Combined Gauss + Erf //////////

double comb(
    double x,
    double A_gauss,
    double A_erf,
    double x0,
    double sigma,
    double offset
) {
    return (
        gaussian(x, A_gauss, x0, sigma) + mod_erf(x, A_erf, x0, sigma, offset)
    );
}

static int comb_f(const gsl_vector *params, void *data, gsl_vector *f) {
    size_t n = ((data_t*)data)->n;
    double *x = ((data_t*)data)->x;
    double *y = ((data_t*)data)->y;

    double A_gaussian = gsl_vector_get(params, 0);
    double A_erf = gsl_vector_get(params, 1);
    double x0 = gsl_vector_get(params, 2);
    double sigma = gsl_vector_get(params, 3);
    double offset  = gsl_vector_get(params, 4);

    if (fabs(sigma) < 1e-10) return GSL_FAILURE;

    for (int i = 0; i < n; i++) {
        double Yi = comb(x[i], A_gaussian, A_erf, x0, sigma, offset);
        gsl_vector_set(f, i, Yi - y[i]);
    }

    return GSL_SUCCESS;
}

static int comb_df(const gsl_vector *params, void *data, gsl_matrix *J) {
    size_t n = ((data_t*)data)->n;
    double *x = ((data_t*)data)->x;
    double *y = ((data_t*)data)->y;

    double A_gauss = gsl_vector_get(params, 0);
    double A_erf = gsl_vector_get(params, 1);
    double x0 = gsl_vector_get(params, 2);
    double sigma = gsl_vector_get(params, 3);
    double offset  = gsl_vector_get(params, 4);

    if (fabs(sigma) < 1e-10) return GSL_FAILURE;

    for (size_t i = 0; i < n; i++) {
        // Gaussian:
        double xi = x[i];
        double zi = (xi - x0) / sigma;
        double ei_gauss = exp(-0.5 * zi * zi);
        double ei_erf = exp(-zi * zi);
        double erfi = erf(zi);
        double c = 2.0 / sqrt(M_PI);

        double df_dA_gauss = ei_gauss;
        double df_dA_erf = erfi;
        double df_dx0_gauss = (A_gauss / sigma) * ei_gauss * zi;
        double df_dx0_erf = -c * A_erf * ei_erf / sigma;
        double df_dsigma_gauss = (A_gauss / sigma) * ei_gauss * zi * zi;
        double df_dsigma_erf = -c * A_erf * ei_erf * zi / sigma;
        double df_doffset = 1;

        gsl_matrix_set(J, i, 0, df_dA_gauss);
        gsl_matrix_set(J, i, 1, df_dA_erf);
        gsl_matrix_set(J, i, 2, df_dx0_gauss + df_dx0_erf);
        gsl_matrix_set(J, i, 3, df_dsigma_gauss + df_dsigma_erf);
        gsl_matrix_set(J, i, 4, df_doffset);
    }

    return GSL_SUCCESS;
}

static void comb_callback(
    const size_t iter, void *params, const gsl_multifit_nlinear_workspace *w
) {
    gsl_vector *f = gsl_multifit_nlinear_residual(w);
    gsl_vector *x = gsl_multifit_nlinear_position(w);
    double rcond;

    /* compute reciprocal condition number of J(x) */
    gsl_multifit_nlinear_rcond(&rcond, w);

    printf(
        "iter %2zu: A_gauss = %.4f, A_erf = %.4f, x0 = %.4f, sigma = %.4f, "
        "offset = %.4f, cond(J) = %8.4f, |f(x)| = %.4f\n",
        iter, gsl_vector_get(x, 0), gsl_vector_get(x, 1),
        gsl_vector_get(x, 2), gsl_vector_get(x, 3), gsl_vector_get(x, 4),
        1.0 / rcond, gsl_blas_dnrm2(f)
    );
}

double *fitComb(double *x, double *y, size_t N, double init[5], int debug) {
    return fit(x, y, N, 5, init, &comb_f, &comb_df, &comb_callback, debug);
}

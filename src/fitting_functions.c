#include <gsl/gsl_blas.h>
#include <gsl/gsl_multifit_nlinear.h>

#include "../include/fitting_functions.h"

#define PI 3.141592653589793

extern int verbose;
extern int debug;

double *fitWrapper(
    double *x,
    double *y,
    const size_t n,
    const size_t p,
    double init[3],
    int (*f)(const gsl_vector *, void *, gsl_vector *),
    int (*df)(const gsl_vector *, void *, gsl_matrix *),
    void (*callback)(
        const size_t, void *, const gsl_multifit_nlinear_workspace *
    )
) {
    const gsl_multifit_nlinear_type *T = gsl_multifit_nlinear_trust;
    gsl_multifit_nlinear_workspace *w;
    gsl_multifit_nlinear_fdf fdf;
    gsl_multifit_nlinear_parameters fdf_params =
        gsl_multifit_nlinear_default_parameters();

    gsl_vector *res;
    gsl_matrix *J;
    gsl_matrix *covar = gsl_matrix_alloc(p, p);
    data_t d = {n, x, y};
    gsl_vector_view params = gsl_vector_view_array(init, p);
    double chisq, chisq0;
    int status, info;
    size_t i;

    const double xtol = 1e-3;
    const double gtol = 1e-3;
    const double ftol = 0.0;

    /* define the function to be minimized */
    fdf.f = f;
    fdf.df = df;    /* set to NULL for finite-difference Jacobian */
    fdf.fvv = NULL; /* not using geodesic acceleration */
    fdf.n = n;
    fdf.p = p;
    fdf.params = &d;

    /* allocate workspace with default parameters */
    w = gsl_multifit_nlinear_alloc(T, &fdf_params, n, p);

    /* initialize solver with starting point and weights */
    gsl_multifit_nlinear_init(&params.vector, &fdf, w);

    /* compute initial cost function */
    res = gsl_multifit_nlinear_residual(w);
    gsl_blas_ddot(res, res, &chisq0);

    /* solve the system with a maximum of 100 iterations */
    if (!debug) callback = NULL;

    status = gsl_multifit_nlinear_driver(
        100, xtol, gtol, ftol, callback, NULL, &info, w
    );

    /* compute covariance of best fit parameters */
    J = gsl_multifit_nlinear_jac(w);
    gsl_multifit_nlinear_covar(J, 0.0, covar);

    /* compute final cost */
    gsl_blas_ddot(res, res, &chisq);

    if (debug) {
#define FIT(i) gsl_vector_get(w->x, i)
#define ERR(i) sqrt(gsl_matrix_get(covar, i, i))

        fprintf(
            stderr, "summary from method '%s/%s'\n",
            gsl_multifit_nlinear_name(w), gsl_multifit_nlinear_trs_name(w)
        );
        fprintf(
            stderr, "number of iterations: %zu\n", gsl_multifit_nlinear_niter(w)
        );
        fprintf(stderr, "function evaluations: %zu\n", fdf.nevalf);
        fprintf(stderr, "Jacobian evaluations: %zu\n", fdf.nevaldf);
        fprintf(
            stderr, "reason for stopping: %s\n",
            (info == 1) ? "small step size" : "small gradient"
        );
        fprintf(stderr, "initial |f(x)| = %f\n", sqrt(chisq0));
        fprintf(stderr, "final   |f(x)| = %f\n", sqrt(chisq));

        {
            double dof = n - p;
            double c = GSL_MAX_DBL(1, sqrt(chisq / dof));

            fprintf(stderr, "chisq/dof = %g\n", chisq / dof);

            for (int i = 0; i < p; i++) {
                fprintf(
                    stderr, "x[%d] = %.5f +/- %.5f\n", i, FIT(i), c * ERR(i)
                );
            }
        }

        fprintf(stderr, "status = %s\n", gsl_strerror(status));
    }

    double *result = (double *)malloc(p * sizeof(double));

    for (int i = 0; i < p; i++) {
        result[i] = gsl_vector_get(w->x, i);
    }

    gsl_multifit_nlinear_free(w);
    gsl_matrix_free(covar);

    return result;
}

double gaussian(double x, double A, double x0, double sigma) {
    return A * exp(-0.5 * pow((x - x0) / sigma, 2));
}

static int gaussian_f(const gsl_vector *params, void *data, gsl_vector *f) {
    size_t n = ((data_t *)data)->n;
    double *x = ((data_t *)data)->x;
    double *y = ((data_t *)data)->y;

    double A = gsl_vector_get(params, 0);
    double x0 = gsl_vector_get(params, 1);
    double sigma = gsl_vector_get(params, 2);

    if (sigma == 0) return GSL_FAILURE;

    for (int i = 0; i < n; i++) {
        double Yi = A * exp(-0.5 * pow((x[i] - x0) / sigma, 2));
        gsl_vector_set(f, i, Yi - y[i]);
    }

    return GSL_SUCCESS;
}

static int gaussian_df(const gsl_vector *params, void *data, gsl_matrix *J) {
    size_t n = ((data_t *)data)->n;
    double *x = ((data_t *)data)->x;

    double A = gsl_vector_get(params, 0);
    double x0 = gsl_vector_get(params, 1);
    double sigma = gsl_vector_get(params, 2);

    if (sigma == 0) return GSL_FAILURE;

    for (size_t i = 0; i < n; i++) {
        double df_dA = exp(-0.5 * pow((x[i] - x0) / sigma, 2));
        double df_dx0 = A * df_dA * (x[i] - x0) / (sigma * sigma);
        double df_dsigma = df_dx0 * (x[i] - x0) / sigma;

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

    if (debug) {
        printf(
            "iter %2zu: A = %.4f, x0 = %.4f, sigma = %.4f, cond(J) = %8.4f, "
            "|f(x)| = %.4f\n",
            iter, gsl_vector_get(x, 0), gsl_vector_get(x, 1),
            gsl_vector_get(x, 2), 1.0 / rcond, gsl_blas_dnrm2(f)
        );
    }
}

double *fitGaussian(double *x, double *y, const size_t N, double init[3]) {
    return fitWrapper(
        x, y, N, 3, init, &gaussian_f, &gaussian_df, &gaussian_callback
    );
}

static int erf_f(const gsl_vector *params, void *data, gsl_vector *f) {
    size_t n = ((data_t *)data)->n;
    double *x = ((data_t *)data)->x;
    double *y = ((data_t *)data)->y;

    double A = gsl_vector_get(params, 0);
    double x0 = gsl_vector_get(params, 1);
    double smooth = gsl_vector_get(params, 2);
    double offset = gsl_vector_get(params, 3);

    for (int i = 0; i < n; i++) {
        double Yi = A * erf(smooth * (x[i] - x0)) + offset;
        gsl_vector_set(f, i, Yi - y[i]);
    }

    return GSL_SUCCESS;
}

static int erf_df(const gsl_vector *params, void *data, gsl_matrix *J) {
    size_t n = ((data_t *)data)->n;
    double *x = ((data_t *)data)->x;

    double A = gsl_vector_get(params, 0);
    double x0 = gsl_vector_get(params, 1);
    double smooth = gsl_vector_get(params, 2);

    for (size_t i = 0; i < n; i++) {
        double df_dA = erf(smooth * (x[i] - x0));
        double df_dx0 =
            (1.128379 * A * exp(-0.5 * pow(smooth * (x[i] - x0), 2)) * smooth *
             smooth * (x[i] - x0));
        double df_dsmooth = -df_dx0 * (x[i] - x0) / smooth;
        double df_doffset = 1;

        gsl_matrix_set(J, i, 0, df_dA);
        gsl_matrix_set(J, i, 1, df_dx0);
        gsl_matrix_set(J, i, 2, df_dsmooth);
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

    if (debug) {
        printf(
            "iter %2zu: A = %.4f, x0 = %.4f, sigma = %.4f, cond(J) = %8.4f, "
            "|f(x)| = %.4f\n",
            iter, gsl_vector_get(x, 0), gsl_vector_get(x, 1),
            gsl_vector_get(x, 2), 1.0 / rcond, gsl_blas_dnrm2(f)
        );
    }
}

double *fitErf(double *x, double *y, const size_t N, double init[3]) {
    return fitWrapper(x, y, N, 3, init, &erf_f, &erf_df, &erf_callback);
}

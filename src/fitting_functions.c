#include "fitting_functions.h"

#include <math.h>

#include <gsl/gsl_multifit_nlinear.h>
#include <gsl/gsl_blas.h>


#define PI 3.141592653589793

int debug = 1;


/**
 * @brief Perform the fit of the given function to the given data
 *
 * @param x pointer to the x data array
 * @param y pointer to the y data array
 * @param n length of x and y arrays
 * @param p number of parameters of the function to be fitted (= length of `vary`)
 * @param vary list of initial values of the params to be varied during fitting
 * @param fix list of values of the params to be fixed during fitting
 * @param f pointer to the function to fit
 * @param df pointer to the derivative of function to fit
 * @param callback pointer to the callback function to call in each fit iteration
 */
void fitWrapper(double *x, double *y, const size_t n, const size_t p,
                 double vary[], double fixed[],
                 int (*f)(const gsl_vector*, void*, gsl_vector*),
                 int (*df)(const gsl_vector*, void*, gsl_matrix*),
                 void (*callback)(const size_t, void*,
                                  const gsl_multifit_nlinear_workspace*))
{
    const gsl_multifit_nlinear_type *T = gsl_multifit_nlinear_trust;
    gsl_multifit_nlinear_workspace *w;
    gsl_multifit_nlinear_fdf fdf;
    gsl_multifit_nlinear_parameters fdf_params = gsl_multifit_nlinear_default_parameters();

    gsl_vector *res;
    gsl_matrix *J;
    gsl_matrix *covar = gsl_matrix_alloc(p, p);
    data_t d = {n, x, y, fixed};
    gsl_vector_view params = gsl_vector_view_array(vary, p);
    double chisq, chisq0;
    int status, info;
    size_t i;

    const double xtol = 1e-6;
    const double gtol = 1e-8;
    const double ftol = 0.0;

    /* define the function to be minimized */
    fdf.f = f;
    fdf.df = df;        /* set to NULL for finite-difference Jacobian */
    fdf.fvv = NULL;     /* not using geodesic acceleration */
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
    if (!debug)
        callback = NULL;

    status = gsl_multifit_nlinear_driver(100, xtol, gtol, ftol, callback, NULL, &info, w);

    /* compute covariance of best fit parameters */
    J = gsl_multifit_nlinear_jac(w);
    gsl_multifit_nlinear_covar(J, 0.0, covar);

    /* compute final cost */
    gsl_blas_ddot(res, res, &chisq);

    if (debug)
    {
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

            for (int i = 0; i < p; i++)
                fprintf (stderr, "x[%d] = %.5f +/- %.5f\n", i, FIT(i), c*ERR(i));
        }

        fprintf (stderr, "status = %s\n", gsl_strerror(status));
    }

    for (int i = 0; i < p; i++)
        vary[i] =  gsl_vector_get(w->x, i);

    gsl_multifit_nlinear_free(w);
    gsl_matrix_free(covar);
}

static int gaussian_f(const gsl_vector *params, void *data, gsl_vector *f)
{
    data_t *d = (data_t*)data;
    size_t n = d->n;
    double *x = d->x;
    double *y = d->y;

    double A = gsl_vector_get(params, 0);
    double x0 = gsl_vector_get(params, 1);
    double inv_sig = gsl_vector_get(params, 2);

    for (int i = 0; i < n; i++)
    {
        double Yi = gaussian(x[i], A, x0, inv_sig);
        gsl_vector_set(f, i, Yi - y[i]);
    }

    return GSL_SUCCESS;
}

static int gaussian_df(const gsl_vector *params, void *data, gsl_matrix *J)
{
    data_t *d = (data_t*)data;
    size_t n = d->n;
    double *x = d->x;

    double A = gsl_vector_get(params, 0);
    double x0 = gsl_vector_get(params, 1);
    double inv_sig = gsl_vector_get(params, 2);

    for (size_t i = 0; i < n; i++)
    {
        double df_dA = gaussian(x[i], 1, x0, inv_sig);
        double h = A * df_dA * (x[i] - x0) * inv_sig;
        double df_dx0 = h * inv_sig;
        double df_dinv_sig = -h * (x[i] - x0);

        gsl_matrix_set(J, i, 0, df_dA);
        gsl_matrix_set(J, i, 1, df_dx0);
        gsl_matrix_set(J, i, 2, df_dinv_sig);
    }

    return GSL_SUCCESS;
}

static void gaussian_callback(const size_t iter, void *params,
                              const gsl_multifit_nlinear_workspace *w)
{
    gsl_vector *f = gsl_multifit_nlinear_residual(w);
    gsl_vector *x = gsl_multifit_nlinear_position(w);
    double rcond;

    /* compute reciprocal condition number of J(x) */
    gsl_multifit_nlinear_rcond(&rcond, w);

    if (debug)
    {
        printf("iter %2zu: A = %.4f, x0 = %.4f, inv_sig = %.4f, cond(J) = %8.4f, "
               "|f(x)| = %.4f\n",
                iter,
                gsl_vector_get(x, 0),
                gsl_vector_get(x, 1),
                gsl_vector_get(x, 2),
                1.0 / rcond,
                gsl_blas_dnrm2(f));
    }
}

void fitGaussian(double *x, double *y, const size_t N, double init[3])
{
    fitWrapper(x, y, N, 3, init, NULL, &gaussian_f, &gaussian_df, &gaussian_callback);
}

static int erf_f(const gsl_vector *params, void *data, gsl_vector *f)
{
    data_t *d = (data_t*)data;
    size_t n = d->n;
    double *x = d->x;
    double *y = d->y;
    double *fixed = d->fixed;

    double A = gsl_vector_get(params, 0);
    double offset = gsl_vector_get(params,1);

    double x0 = fixed[0];
    double inv_sig = fixed[1];

    for (int i = 0; i < n; i++)
    {
        double Yi = my_erf(x[i], A, x0, inv_sig, offset);
        gsl_vector_set(f, i, Yi - y[i]);
    }

    return GSL_SUCCESS;
}

static int erf_df(const gsl_vector *params, void *data, gsl_matrix *J)
{
    data_t *d = (data_t*)data;
    size_t n = d->n;
    double *x = d->x;
    double *fixed = d->fixed;

    double A = gsl_vector_get(params, 0);

    double x0 = fixed[0];
    double inv_sig = fixed[1];

    for (size_t i = 0; i < n; i++)
    {
        double df_dA = erf(inv_sig * (x[i] - x0));
        //double df_dinv_sig = 1.128379 * gaussian(x[i], A, x0, 1.4142 * inv_sig) * (x[i] - x0);
        double df_doffset = 1;

        gsl_matrix_set(J, i, 0, df_dA);
        gsl_matrix_set(J, i, 1, df_doffset);
    }

    return GSL_SUCCESS;
}

static void erf_callback(const size_t iter, void *params,
                         const gsl_multifit_nlinear_workspace *w)
{
    gsl_vector *f = gsl_multifit_nlinear_residual(w);
    gsl_vector *x = gsl_multifit_nlinear_position(w);
    double rcond;

    /* compute reciprocal condition number of J(x) */
    gsl_multifit_nlinear_rcond(&rcond, w);

    if (debug)
    {
        printf("iter %2zu: A = %.4f, offset = %.4f, cond(J) = %8.4f, "
               "|f(x)| = %.4f\n",
                iter,
                gsl_vector_get(x, 0),
                gsl_vector_get(x, 1),
                1.0 / rcond,
                gsl_blas_dnrm2(f));
    }
}

void fitErf(double *x, double *y, const size_t N, double init[4])
{
    double vary[2] = {init[0], init[3]};
    double fixed[2] = {init[1], init[2]};

    fitWrapper(x, y, N, 2, vary, fixed, &erf_f, &erf_df, &erf_callback);

    init[0] = vary[0];
    init[3] = vary[1];
}

static int gauss_erf_f(const gsl_vector *params, void *data, gsl_vector *f)
{
    data_t *d = (data_t*)data;
    size_t n = d->n;
    double *x = d->x;
    double *y = d->y;
    double *fixed = d->fixed;

    double gauss_amp = gsl_vector_get(params, 0);
    double x0 = gsl_vector_get(params, 1);
    double gauss_inv_sig = gsl_vector_get(params, 2);

    double erf_amp = fixed[0];
    double erf_inv_sig = fixed[1];
    double offset = fixed[2];

    for (int i = 0; i < n; i++)
    {
        double Yi = gauss_erf(x[i], gauss_amp, x0, gauss_inv_sig, erf_amp, erf_inv_sig, offset);
        gsl_vector_set(f, i, Yi - y[i]);
    }

    return GSL_SUCCESS;
}

static int gauss_erf_df(const gsl_vector *params, void *data, gsl_matrix *J)
{
    data_t *d = (data_t*)data;
    size_t n = d->n;
    double *x = d->x;
    double *fixed = d->fixed;

    double gauss_amp = gsl_vector_get(params, 0);
    double x0 = gsl_vector_get(params, 1);
    double gauss_inv_sig = gsl_vector_get(params, 2);

    double erf_amp = fixed[0];
    double erf_inv_sig = fixed[1];
    double offset = fixed[2];

    for (size_t i = 0; i < n; i++)
    {
        double df_dgauss_amp = gaussian(x[i], 1, x0, gauss_inv_sig);
        double h = gauss_amp * df_dgauss_amp * (x[i] - x0) * gauss_inv_sig;
        double df_dx0 = h * gauss_inv_sig - 1.128379 * gaussian(x[i], erf_amp, x0, erf_inv_sig) * erf_inv_sig;
        double df_dinv_sig = -h * (x[i] - x0);

        gsl_matrix_set(J, i, 0, df_dgauss_amp);
        gsl_matrix_set(J, i, 1, df_dx0);
        gsl_matrix_set(J, i, 2, df_dinv_sig);
    }

    return GSL_SUCCESS;
}

static void gauss_erf_callback(const size_t iter, void *params,
                               const gsl_multifit_nlinear_workspace *w)
{
    gsl_vector *f = gsl_multifit_nlinear_residual(w);
    gsl_vector *x = gsl_multifit_nlinear_position(w);
    double rcond;

    /* compute reciprocal condition number of J(x) */
    gsl_multifit_nlinear_rcond(&rcond, w);

    if (debug)
    {
        printf("iter %2zu: gauss_amp = %.4f, x0 = %.4f, gauss_inv_sig = %.4f, cond(J) = %8.4f, "
               "|f(x)| = %.4f\n",
               iter,
               gsl_vector_get(x, 0),
               gsl_vector_get(x, 1),
               gsl_vector_get(x, 2),
               1.0 / rcond,
               gsl_blas_dnrm2(f));
    }
}

void fitGaussErf(double *x, double *y, const size_t N, double init[6])
{
    // Init contains: gauss_amp, center, gauss_inv_sig, erf_amp, erf_inv_sig, offset
    double vary[3] = {init[0], init[1], init[2]};
    double fixed[3] = {init[3], init[4], init[5]};

    fitWrapper(x, y, N, 3, vary, fixed, &gauss_erf_f, &gaussian_df, &gauss_erf_callback);

    init[0] = vary[0];
    init[1] = vary[1];
    init[2] = vary[2];
}

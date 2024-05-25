#ifndef FITTING_FUNCTIONS_H
#define FITTING_FUNCTIONS_H

#include <gsl/gsl_multifit_nlinear.h>
#include <gsl/gsl_blas.h>

typedef struct
{
    size_t n;
    double *x;
    double *y;
} data_t;

double *fitWrapper(double *x, double *y, const size_t n, const size_t p,
                   double init[3],
                   int (*f)(const gsl_vector*, void*, gsl_vector*),
                   int (*df)(const gsl_vector*, void*, gsl_matrix*),
                   void (*callback)(const size_t, void*,
                                    const gsl_multifit_nlinear_workspace*));

double gaussian(double x, double A, double x0, double sigma);

static int gaussian_f(const gsl_vector *params, void *data, gsl_vector *f);

static int gaussian_df(const gsl_vector *params, void *data, gsl_matrix *J);

static void gaussian_callback(const size_t iter, void *params,
                              const gsl_multifit_nlinear_workspace *w);

double *fitGaussian(double *x, double *y, const size_t N, double init[3]);

static int erf_f(const gsl_vector *params, void *data, gsl_vector *f);

static int erf_df(const gsl_vector *params, void *data, gsl_matrix *J);

static void erf_callback(const size_t iter, void *params,
                         const gsl_multifit_nlinear_workspace *w);

double *fitErf(double *x, double *y, const size_t N, double init[3]);

#endif

#ifndef FITTING_FUNCTIONS_H
#define FITTING_FUNCTIONS_H

#ifdef __cplusplus
extern "C" {
#endif

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

double *fitGaussian(double *x, double *y, const size_t N, double init[3]);

double *fitErf(double *x, double *y, const size_t N, double init[3]);

#ifdef __cplusplus
}
#endif

#endif

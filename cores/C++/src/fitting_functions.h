#ifndef FITTING_FUNCTIONS_H
#define FITTING_FUNCTIONS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <math.h>

#include <gsl/gsl_multifit_nlinear.h>
#include <gsl/gsl_blas.h>

typedef struct {
    size_t n;
    double *x;
    double *y;
    double *fixed;
} data_t;

void fitWrapper(double *x, double *y, const size_t n, const size_t p,
                 double vary[], double fixed[],
                 int (*f)(const gsl_vector*, void*, gsl_vector*),
                 int (*df)(const gsl_vector*, void*, gsl_matrix*),
                 void (*callback)(const size_t, void*,
                                  const gsl_multifit_nlinear_workspace*));

inline double gaussian(double x, double A, double x0, double inv_sig)
{
    return A * exp(-0.5 * pow((x - x0) * inv_sig, 2));
}

void fitGaussian(double *x, double *y, const size_t N, double init[3]);

inline double my_erf(double x, double A, double x0, double inv_sig, double offset)
{
    return A * erf(inv_sig * (x - x0)) + offset;
}

void fitErf(double *x, double *y, const size_t N, double init[4]);

inline double gauss_erf(double x, double gauss_amp, double x0, double gauss_inv_sig,
                        double erf_amp, double erf_inv_sig, double offset)
{
    return gaussian(x, gauss_amp, x0, gauss_inv_sig) + my_erf(x, erf_amp, x0, erf_inv_sig, offset);
}

void fitGaussErf(double *x, double *y, const size_t N, double init[6]);

#ifdef __cplusplus
}
#endif

#endif

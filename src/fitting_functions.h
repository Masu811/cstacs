#ifndef FITTING_FUNCTIONS_H
#define FITTING_FUNCTIONS_H

#ifdef __cplusplus
extern "C" {
#endif

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

inline double gaussian(double x, double A, double x0, double inv_sig);

void fitGaussian(double *x, double *y, const size_t N, double init[3]);

inline double my_erf(double x, double A, double x0, double inv_sig, double offset);

void fitErf(double *x, double *y, const size_t N, double init[4]);

inline double gauss_erf(double x, double gauss_amp, double x0, double gauss_inv_sig,
                        double erf_amp, double erf_inv_sig, double offset);

void fitGaussErf(double *x, double *y, const size_t N, double init[6]);

#ifdef __cplusplus
}
#endif

#endif

#ifndef FITTING_FUNCTIONS_H
#define FITTING_FUNCTIONS_H

#include <gsl/gsl_blas.h>
#include <gsl/gsl_multifit_nlinear.h>

typedef struct {
    size_t n;
    double *x;
    double *y;
} data_t;

typedef struct {
    gsl_multifit_nlinear_workspace *w;
    gsl_multifit_nlinear_fdf fdf;
    gsl_matrix *covar;
    data_t d;
    double chisq, chisq0;
    int status, info;
} FitEnv;

double gaussian(double x, double A, double x0, double sigma);

double *fitGaussian(
    double *x, double *y, size_t N, double init[3], int debug
);

double mod_erf(double x, double A, double x0, double sigma, double offset);

double *fitErf(
    double *x, double *y, size_t N, double init[4], int debug
);

double comb(
    double x,
    double A_gauss,
    double A_erf,
    double x0,
    double sigma,
    double offset
);

double *fitComb(double *x, double *y, size_t N, double init[5], int debug);

#endif

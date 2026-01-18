#include <criterion/criterion.h>
#include <criterion/internal/assert.h>

#include <stdio.h>

#include "../include/fitting.h"

Test(test_fitting, test_fitGaussian) {
    int N = 1000;

    double A = 1000;
    double x0 = N / 2;
    // typically 1000 keV onto 16k channels
    // 1.5 keV eres => sigma = 24 / 2.35 = 10.2 channels
    double sigma = 10.2;

    double *x = (double*)malloc(N * sizeof(double));
    double *y = (double*)malloc(N * sizeof(double));

    for (int i = 0; i < N; i++) {
        x[i] = i;
        y[i] = gaussian(x[i], A, x0, sigma);
    }

    FILE *f = fopen("output/gauss.csv", "w");

    if (f != NULL) {
        for (int i = 0; i < N; i++) {
            fprintf(f, "%f,%f\n", x[i], y[i]);
        }
        fclose(f);
    }

    double init[3] = {A + 10, x0 + 1, sigma + 2};

    double *result = fitGaussian(x, y, N, init, 1);

    printf("%f, %f, %f\n", result[0], result[1], result[2]);

    cr_expect_float_eq(result[0], A, 1e-3);
    cr_expect_float_eq(result[1], x0, 1e-3);
    cr_expect_float_eq(result[2], sigma, 1e-3);

    free(result);
    free(x);
    free(y);
}

Test(test_fitting, test_fitErf) {
    int N = 1000;

    double A = 100;
    double x0 = N / 2;
    // typically 1000 keV onto 16k channels
    // 1.5 keV eres => sigma = 24 / 2.35 = 10.2 channels
    double sigma = 10.2;
    double offset = 10;

    double *x = (double*)malloc(N * sizeof(double));
    double *y = (double*)malloc(N * sizeof(double));

    for (int i = 0; i < N; i++) {
        x[i] = i;
        y[i] = mod_erf(x[i], A, x0, sigma, offset);
    }

    FILE *f = fopen("output/erf.csv", "w");

    if (f != NULL) {
        for (int i = 0; i < N; i++) {
            fprintf(f, "%f,%f\n", x[i], y[i]);
        }
        fclose(f);
    }

    double init[4] = {A + 10, x0 + 1, sigma + 2, offset + 2};

    double *result = fitErf(x, y, N, init, 1);

    printf("%f, %f, %f, %f\n", result[0], result[1], result[2], result[3]);

    cr_expect_float_eq(result[0], A, 1e-3);
    cr_expect_float_eq(result[1], x0, 1e-3);
    cr_expect_float_eq(result[2], sigma, 1e-3);
    cr_expect_float_eq(result[3], offset, 1e-3);

    free(result);
    free(x);
    free(y);
}

Test(test_fitting, test_fitComb) {
    int N = 1000;

    double A_gauss = 1000;
    double A_erf = -100;
    double x0 = N / 2;
    // typically 1000 keV onto 16k channels
    // 1.5 keV eres => sigma = 24 / 2.35 = 10.2 channels
    double sigma = 10.2;
    double offset = 100;

    double *x = (double*)malloc(N * sizeof(double));
    double *y = (double*)malloc(N * sizeof(double));

    for (int i = 0; i < N; i++) {
        x[i] = i;
        y[i] = comb(x[i], A_gauss, A_erf, x0, sigma, offset);
    }

    FILE *f = fopen("output/comb.csv", "w");

    if (f != NULL) {
        for (int i = 0; i < N; i++) {
            fprintf(f, "%f,%f\n", x[i], y[i]);
        }
        fclose(f);
    }

    double init[5] = {A_gauss + 10, A_erf + 5, x0 + 1, sigma + 2, offset + 2};

    double *result = fitComb(x, y, N, init, 1);

    printf(
        "%f, %f, %f, %f, %f\n",
        result[0], result[1], result[2], result[3], result[4]
    );

    cr_expect_float_eq(result[0], A_gauss, 1e-3);
    cr_expect_float_eq(result[1], A_erf, 1e-3);
    cr_expect_float_eq(result[2], x0, 1e-3);
    cr_expect_float_eq(result[3], sigma, 1e-3);
    cr_expect_float_eq(result[4], offset, 1e-3);

    free(result);
    free(x);
    free(y);
}

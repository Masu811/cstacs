#include <criterion/criterion.h>
#include <criterion/internal/assert.h>
#include <stdbool.h>

#include "../src/single.c"

Test(test_singles, test_calcEnergies) {
    SingleSpectrum s = {0};

    s.ecal.values[0] = 1;
    s.ecal.values[1] = 2;

    s.spectrum = (int[]){1, 2, 3};
    s.spectrum_size = 3;
    s.ecal_found = true;

    calcEnergies(&s);

    cr_expect_float_eq(s.energies[0], 1, 1e-3);
    cr_expect_float_eq(s.energies[1], 3, 1e-3);
    cr_expect_float_eq(s.energies[2], 5, 1e-3);

    free(s.energies);
}

Test(test_singles, test_intToDouble) {
    int n = 10;

    int *intVersion = (int*)malloc(n * sizeof(int));

    for (int i = 0; i < n; i++) {
        intVersion[i] = i;
    }

    double *doubleVersion = intToDouble(intVersion, n);

    for (int i = 0; i < n; i++) {
        cr_expect_float_eq(intVersion[i], doubleVersion[i], 1e-3);
    }

    free(intVersion);
    free(doubleVersion);
}

size_t searchsorted(double *arr, size_t size, double value) {
    for (size_t i = 0; i < size; i++) {
        if (arr[i] > value) return i;
    }
    return size;
}

Test(test_singles, test_searchlinear) {
    double ecal[2] = {1, 2};

    double values[] = {-10, 0, 1, 2, 23, 198, 199, 200, 201};
    int n_values = sizeof(values) / sizeof(values[0]);

    int arr_size = 100;

    double *arr = (double*)malloc(arr_size * sizeof(double));

    for (int i = 0; i < arr_size; i++) {
        arr[i] = ecal[0] + ecal[1] * i;
    }

    for (int i = 0; i < n_values; i++) {
        cr_expect_eq(
            searchlinear(arr, arr_size, ecal, values[i]),
            searchsorted(arr, arr_size, values[i])
        );
    }

    free(arr);
}

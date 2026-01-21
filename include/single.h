#ifndef SINGLE_H
#define SINGLE_H

#include "structs.h"

#define M_POSITRON_keV 510.99895

static int calcEnergies(SingleSpectrum *s);

void plot(
    char *headers[], double *columns[], const int n_cols, const int n_rows
);

int analyze(
    SingleSpectrum *s,
    const double s_width,
    const double w_width,
    const double w_dist,
    const bool w_rightonly,
    const double peak_width,
    const double bg_frac,
    const bool bg_corr,
    double v2p_bounds[4],
    const unsigned int follow_peak_order
);

#endif

#ifndef MEASUREMENT_H
#define MEASUREMENT_H

#include "structs.h"

MeasurementCampaign *importMeasurementCampaign(char *directory, bool debug);

void evaluateSingleSpectrum(
    SingleSpectrum *s,
    double s_width,
    double w_width,
    double w_dist,
    bool w_rightonly,
    double peak_width,
    double bg_frac,
    bool bg_corr,
    double v2p_bounds[4],
    unsigned int follow_peak_order,
    bool verbose,
    bool debug
);

void evaluateCoincidenceSpectrum(CoincidenceSpectrum *c);

void evaluateDopplerMeasurement(
    DopplerMeasurement *dm,
    double s_width,
    double w_width,
    double w_dist,
    bool w_rightonly,
    double peak_width,
    double bg_frac,
    bool bg_corr,
    double v2p_bounds[4],
    unsigned int follow_peak_order,
    bool verbose,
    bool debug
);

void evaluateMeasurementCampaign(
    MeasurementCampaign *mc,
    double s_width,
    double w_width,
    double w_dist,
    bool w_rightonly,
    double peak_width,
    double bg_frac,
    bool bg_corr,
    double v2p_bounds[4],
    unsigned int follow_peak_order,
    bool verbose,
    bool debug
);

void printMeasurementCampaign(MeasurementCampaign *mc);

void freeMeasurementCampaign(MeasurementCampaign *mc);

void showCoincidenceSpectrum(CoincidenceSpectrum *c, const int plot_width);

#endif

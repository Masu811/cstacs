#ifndef MEASUREMENT_H
#define MEASUREMENT_H

#include "structs.h"

MeasurementCampaign *importMeasurementCampaign(char *directory);

void evaluateSingleSpectrum(SingleSpectrum *s);

void evaluateCoincidenceSpectrum(CoincidenceSpectrum *c);

void evaluateDopplerMeasurement(DopplerMeasurement *dm);

void evaluateMeasurementCampaign(MeasurementCampaign *mc);

void printMeasurementCampaign(MeasurementCampaign *mc);

void freeMeasurementCampaign(MeasurementCampaign *mc);

void showCoincidenceSpectrum(CoincidenceSpectrum *c, const int plot_width);

#endif
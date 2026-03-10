#ifndef IMPORTER_H
#define IMPORTER_H

#include <stdbool.h>

#include "structs.h"

char *concatPath(char *directory, char *filename);

void printDopplerMeasurement(DopplerMeasurement *dm);

DopplerMeasurement *import_n42(char *directory, char *filepath, bool debug);

void freeDopplerMeasurement(DopplerMeasurement *dm);

#endif

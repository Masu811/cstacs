#ifndef IMPORTER_H
#define IMPORTER_H

#include "structs.h"

static metadata_item*
getItem(const metadata_item* metadata, const char* key);

static int
onlyTextChildren(const xmlNodePtr node);

static metadata_item*
insertAttr(const xmlAttrPtr attr, const xmlNodePtr next);

static metadata_item*
loadxmlTree(const xmlNodePtr root);

static void
printMetadata(const metadata_item* metadata);

static void
freeMetadata(metadata_item* metadata);

static void
spectrumStrToArr(char* spectrum, SingleSpectrum* s);

static void
import_png(char* filename, CoincidenceSpectrum* coinc);

static DopplerMeasurement*
metadataToDoppler(metadata_item* metadata, const char* filename);

void
printDopplerMeasurement(const DopplerMeasurement* dm);

DopplerMeasurement*
import_n42(const char* filepath, const int verbose);

void
freeDopplerMeasurement(DopplerMeasurement* dm);

#endif

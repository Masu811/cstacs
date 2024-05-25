#ifndef IMPORTER_H
#define IMPORTER_H

#include <libxml/parser.h>
#include <libxml/tree.h>

#include "structs.h"

static metadata_item *getItem(metadata_item *metadata, char *key);

static int onlyTextChildren(xmlNodePtr node);

static metadata_item *insertAttr(xmlAttrPtr attr, xmlNodePtr next);

static metadata_item *loadxmlTree(xmlNodePtr root);

static void printMetadata(metadata_item *metadata);

static void freeMetadata(metadata_item *metadata);

static void spectrumStrToArr(char *spectrum, SingleSpectrum *s);

static void import_png(char *filename, CoincidenceSpectrum *coinc);

char *concatPath(char *directory, char *filename);

static DopplerMeasurement *metadataToDoppler(metadata_item *metadata,
                                             char *directory, char *filename);

void printDopplerMeasurement(DopplerMeasurement *dm);

DopplerMeasurement *import_n42(char *directory, char *filepath);

void freeDopplerMeasurement(DopplerMeasurement *dm);

#endif

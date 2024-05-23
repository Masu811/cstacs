#ifndef STRUCTS_H
#define STRUCTS_H

#include <stdlib.h>

typedef struct metadata_item {
    char *key;
    char *value;
    struct metadata_item *children;
    struct metadata_item *next;
} metadata_item;

typedef struct SingleSpectrum {
    int *spectrum;
    int spectrum_size;
    double *energies;
    char *filename;
    union {
        char *ref;
        char *name;
    } detname;
    union {
        char *ref;
        double values[2];
    } ecal;
    double eres;
    unsigned long long counts;
    double dcounts;
    double s;
    double ds;
    double w;
    double dw;
    double v2p;
    double dv2p;
    unsigned long long peak_counts;
    double dpeak_counts;
} SingleSpectrum;

typedef struct CoincidenceSpectrum {
    int *spectrum;
    double *energies_1;
    double *energies_2;
    int width;
    int height;
    char *filename;
    char *parentname;
    union {
        char *ref;
        char *name;
    } detpair;
    int *window;
    double coinc_time;
    double eres;
    unsigned long long counts;
    double dcounts;
    double s;
    double ds;
    double w;
    double dw;
    unsigned long long peak_counts;
    double dpeak_counts;
} CoincidenceSpectrum;

typedef struct DopplerMeasurement {
    char *filename;
    SingleSpectrum **singles;
    int n_singles;
    CoincidenceSpectrum **coinc;
    int n_coinc;
    metadata_item *metadata;
} DopplerMeasurement;

typedef struct MeasurementCampaign {
    DopplerMeasurement **measurements;
    int n_measurements;
} MeasurementCampaign;

#endif


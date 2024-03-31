#ifndef STRUCTS_H
#define STRUCTS_H

#include <stdlib.h>

typedef struct metadata_item {
    char* key;
    char* value;
    struct metadata_item* children;
    struct metadata_item* next;
} metadata_item;

typedef struct SingleSpectrum {
    int* spectrum;
    int spectrum_size;
    char* filename;
    union {
        char* ref;
        char* name;
    } detname;
    union {
        char* ref;
        float values[2];
    } ecal;
    float eres;
    unsigned long counts;
    float dcounts;
    float s;
    float ds;
    float w;
    float dw;
    float v2p;
    float dv2p;
    unsigned long peak_counts;
    float dpeak_counts;
} SingleSpectrum;

typedef struct CoincidenceSpectrum {
    int** spectrum;
    int* spectrum_size;
    char* filename;
    union {
        char* ref;
        char* name;
    } detpair;
    int* window;
    float coinc_time;
    float eres;
    unsigned long counts;
    float dcounts;
    float s;
    float ds;
    float w;
    float dw;
    unsigned long peak_counts;
    float dpeak_counts;
} CoincidenceSpectrum;

typedef struct DopplerMeasurement {
    char* filename;
    SingleSpectrum* singles;
    int n_singles;
    CoincidenceSpectrum* coinc;
    int n_coinc;
    metadata_item* metadata;
} DopplerMeasurement;

typedef struct MeasurementCampaign {
    DopplerMeasurement** measurements;
    int n_measurements;
} MeasurementCampaign;

#endif


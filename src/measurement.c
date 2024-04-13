#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <math.h>
#include <time.h>

#include "structs.h"
#include "importer.h"



void
progress(const int done, const int total, const int bar_length)
{
    printf("[");
    float fraction = (float) done / total;
    for (int i = 0; i < (int)(fraction * bar_length); i++) {
        printf("#");
    }
    for (int i = 0; i < (int)((1 - fraction) * bar_length); i++) {
        printf(" ");
    }
    printf("] %d / %d (%.1f %%)", done, total, 100 * fraction);
}

MeasurementCampaign*
importMeasurementCampaign(const char* directory)
{
    MeasurementCampaign* mc = ((MeasurementCampaign*)
                               calloc(1, sizeof(MeasurementCampaign)));
    mc->measurements = ((DopplerMeasurement**)
                        calloc(1, sizeof(DopplerMeasurement*)));
    DIR *dir;
    struct dirent *entry;

    dir = opendir(directory);

    if (dir == NULL) {
        perror("opendir");
        return NULL;
    }

    int l = strlen(directory);
    int n_files = 0;

    while ((entry = readdir(dir)) != NULL) {
        if (strstr(entry->d_name, ".n42\0") != NULL) {
            n_files++;
        }
    }

    rewinddir(dir);

    int file_idx = 0;

    while ((entry = readdir(dir)) != NULL) {
        if (strstr(entry->d_name, ".n42\0") != NULL) {
            int i = mc->n_measurements;
            DopplerMeasurement* dm = import_n42(directory, entry->d_name, 0);
            mc->measurements = ((DopplerMeasurement**)
                                realloc(mc->measurements,
                                        (i + 1) * sizeof(DopplerMeasurement*)));
            mc->measurements[i] = dm;
            mc->n_measurements++;
            file_idx++;
            printf("\rInitializing Measurement Campaign: ");
            progress(file_idx, n_files, 30);
        }
    }
    puts("");

    closedir(dir);

    return mc;
}

void
evaluateSingleSpectrum(SingleSpectrum* s)
{
   for (int i = 0; i < s->spectrum_size; i++) {
      s->counts = s->counts + s->spectrum[i];
    }

   s->dcounts = sqrt(s->counts);

   return;
}

void
evaluateCoincidenceSpectrum(CoincidenceSpectrum* c)
{
   for (int i = 0; i < c->width * c->height; i++) {
      c->counts = c->counts + c->spectrum[i];
    }

   c->dcounts = sqrt(c->counts);

   return;
}

void
evaluateDopplerMeasurement(DopplerMeasurement* dm)
{
    for (int i = 0; i < dm->n_singles; i++) {
        evaluateSingleSpectrum(dm->singles[i]);
    }
    for (int i = 0; i < dm->n_coinc; i++) {
        evaluateCoincidenceSpectrum(dm->coinc[i]);
    }

    return;
}

void
evaluateMeasurementCampaign(MeasurementCampaign* mc)
{
    for (int i = 0; i < mc->n_measurements; i++) {
        evaluateDopplerMeasurement(mc->measurements[i]);
    }

    return;
}

void
printMeasurementCampaign(MeasurementCampaign* mc)
{
    printf("MeasurementCampaign contains %d DopplerMeasurements:\n",
            mc->n_measurements);
    for (int i = 0; i < mc->n_measurements; i++) {
        printDopplerMeasurement(mc->measurements[i]);
    }

    return;
}

void
freeMeasurementCampaign(MeasurementCampaign* mc)
{
    for (int i = 0; i < mc->n_measurements; i++) {
        freeDopplerMeasurement(mc->measurements[i]);
    }
    free(mc->measurements);
    free(mc);

    return;
}

void
showCoincidenceSpectrum(CoincidenceSpectrum* c, const int plot_width)
{
    float step_width = (float)c->width / plot_width;
    int plot_height = c->height / plot_width;

    int max_counts = 0;
    for (int j = 0; j < c->height; j++) {
        for (int i = 0; i < c->width; i++) {
            if (c->spectrum[j * c->width + i] > max_counts) {
                max_counts = c->spectrum[j * c->width + i];
            }
        }
    }

    for (float y = 0; y < c->height; y = y + step_width) {
        for (float x = 0; x < c->width; x = x + step_width) {
            printf("%d ", 10 * (int)c->spectrum[(int)y * c->width + (int)x] / max_counts);
        }
        printf("\n");
    }

    printf("%d\n", max_counts);

    return;
}



int
main(int argc, char** argv)
{
   int verbose = 0;

    if (argc > 1) {
        if (atoi(argv[1]) == 1) {
            verbose = 1;
            puts("Debugging mode enabled");
        }
    }

    clock_t start, stop;
    double cpu_time;

    start = clock();
    // must end with "/"
    char* filepath = ("/home/msuhr/Labbeam/Measurements/2024-01-04_depth-profile_Copper/");

    MeasurementCampaign* mc = importMeasurementCampaign(filepath);
    evaluateMeasurementCampaign(mc);
    printMeasurementCampaign(mc);
    freeMeasurementCampaign(mc);

    stop = clock();
    cpu_time = 1000 * ((double)(stop - start)) / CLOCKS_PER_SEC;

    printf("CPU time used: %f ms\n", cpu_time);

    return 0;
}


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <math.h>
#include <time.h>

#include "structs.h"
#include "importer.h"
#include "single.h"


static void
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

MeasurementCampaign
*importMeasurementCampaign(const char *directory)
{
    MeasurementCampaign *mc = ((MeasurementCampaign*)
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
            DopplerMeasurement *dm = import_n42(directory, entry->d_name, 0);
            mc->measurements = ((DopplerMeasurement**)
                                realloc(mc->measurements,
                                    (i + 1) * sizeof(DopplerMeasurement*)));
            mc->measurements[i] = dm;
            mc->n_measurements++;
            file_idx++;
            printf("\rImporting Measurement Campaign: ");
            progress(file_idx, n_files, 30);
        }
    }
    puts("");

    closedir(dir);

    return mc;
}

void
evaluateSingleSpectrum(SingleSpectrum *s, const int verbose)
{
   for (int i = 0; i < s->spectrum_size; i++) {
      s->counts = s->counts + s->spectrum[i];
    }

   s->dcounts = sqrt(s->counts);

   double v2p_bounds[] = {450, 500, 506, 516};

   analyze(s, 1.1, 1.0, 3.0, 0, 60.0, 0, 1, v2p_bounds, 0, verbose, 0);
}

void
evaluateCoincidenceSpectrum(CoincidenceSpectrum *c, const int verbose)
{
   for (int i = 0; i < c->width * c->height; i++) {
      c->counts = c->counts + c->spectrum[i];
    }

   c->dcounts = sqrt(c->counts);
}

void
evaluateDopplerMeasurement(DopplerMeasurement *dm, const int verbose)
{
    for (int i = 0; i < dm->n_singles; i++) {
        evaluateSingleSpectrum(dm->singles[i], verbose);
    }
    for (int i = 0; i < dm->n_coinc; i++) {
        evaluateCoincidenceSpectrum(dm->coinc[i], verbose);
    }
}

void
evaluateMeasurementCampaign(MeasurementCampaign *mc, const int verbose)
{
    puts("Analyzing Measurement Campaign...");
    for (int i = 0; i < mc->n_measurements; i++) {
        evaluateDopplerMeasurement(mc->measurements[i], verbose);
    }
}

void
printMeasurementCampaign(MeasurementCampaign *mc)
{
    printf("MeasurementCampaign contains %d DopplerMeasurements:\n",
            mc->n_measurements);
    for (int i = 0; i < mc->n_measurements; i++) {
        printDopplerMeasurement(mc->measurements[i]);
    }
}

void
freeMeasurementCampaign(MeasurementCampaign *mc)
{
    for (int i = 0; i < mc->n_measurements; i++) {
        freeDopplerMeasurement(mc->measurements[i]);
    }
    free(mc->measurements);
    free(mc);
}

void
showCoincidenceSpectrum(CoincidenceSpectrum *c, const int plot_width)
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
            int d = 10 * c->spectrum[(int)y * c->width + (int)x] / max_counts;
            printf("%d ", d);
        }
        printf("\n");
    }

    printf("%d\n", max_counts);
}



int
main(int argc, char **argv)
{
   int verbose = 0;

    if (argc < 2) {
        puts("Usage: ./measurement <filepath>");
        return 1;
    }

    clock_t start, stop;
    double cpu_time;

    start = clock();

    MeasurementCampaign *mc = importMeasurementCampaign(argv[1]);
    evaluateMeasurementCampaign(mc, 1);
    //printMeasurementCampaign(mc);
    freeMeasurementCampaign(mc);

    stop = clock();
    cpu_time = 1000 * ((double)(stop - start)) / CLOCKS_PER_SEC;

    printf("CPU time used: %f ms\n", cpu_time);

    return 0;
}


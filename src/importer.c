#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <libxml/parser.h>
#include <libxml/tree.h>

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

DopplerMeasurement*
init_DopplerMeasurement();

static void
spectrumStrToArr(char* spectrum, SingleSpectrum* s);

static DopplerMeasurement*
metadataToDoppler(metadata_item* metadata, const char* filename);

void
printDopplerMeasurement(const DopplerMeasurement* dm);

DopplerMeasurement*
import_n42(const char* filepath, const int verbose);

void
freeDopplerMeasurement(DopplerMeasurement* dm);



static metadata_item*
getItem(const metadata_item* metadata, const char* key)
{
    if (metadata == NULL) {
        return NULL;
    } else if (metadata->children == NULL) {
        return NULL;
    }

    for (metadata_item* child = metadata->children;
            child != NULL; child = child->next) {
        if (!strcmp(child->key, key)) {
            return child;
        }
    }

    return NULL;
}

static int
onlyTextChildren(const xmlNodePtr node)
{
    int textChildrenOnly = 1;

    for (xmlNodePtr child = node->children;
            child != NULL; child = child->next) {
        if (child->type != XML_TEXT_NODE) {
            textChildrenOnly = 0;
            break;
        }
    }

    return textChildrenOnly;
}

static metadata_item*
insertAttr(const xmlAttrPtr attr, const xmlNodePtr next)
{
    if (attr == NULL) {
        return NULL;
    }

    metadata_item* metadata = (metadata_item*)malloc(sizeof(metadata_item));

    if (metadata == NULL) {
        puts("Memory allocation for metadata failed");
        return NULL;
    }

    metadata->key = strdup(attr->name);
    metadata->value = strdup(attr->children->content);
    metadata->children = NULL;

    if (attr->next == NULL) {
        metadata->next = loadxmlTree(next);
    } else {
        metadata->next = insertAttr(attr->next, next);
    }

    return metadata;
}

static metadata_item*
loadxmlTree(const xmlNodePtr root)
{
    if (root == NULL) {
        return NULL;
    } else if (root->type == XML_TEXT_NODE) {
        return loadxmlTree(root->next);
    }

    metadata_item* metadata = (metadata_item*)malloc(sizeof(metadata_item));

    if (metadata == NULL) {
        puts("Memory allocation for metadata failed");
        return NULL;
    }

    if (root->name != NULL) {
        metadata->key = strdup(root->name);
    } else {
        metadata->key = NULL;
    }

    metadata->value = NULL;

    if (root->properties != NULL) {
        metadata->children = insertAttr(root->properties, root->children);
    } else if (root->children == NULL) {
        metadata->children = NULL;
    } else if (onlyTextChildren(root)){
        metadata->children = NULL;
        metadata->value = strdup(root->children->content);
    } else {
        metadata->children = loadxmlTree(root->children);
    }

    metadata->next = loadxmlTree(root->next);

    return metadata;
}

static void
printMetadata(const metadata_item* metadata)
{
    if (metadata == NULL) {
        return;
    }

    printf("%s: ", metadata->key);

    if (metadata->value != NULL) {
        printf("%s\n", metadata->value);
    } else {
        puts("");
    }

    printMetadata(metadata->children);

    printMetadata(metadata->next);

    return;
}

static void
freeMetadata(metadata_item* metadata)
{
    if (metadata->key != NULL) {
        free(metadata->key);
    }
    if (metadata->value != NULL) {
        free(metadata->value);
    }
    if (metadata->children != NULL) {
        freeMetadata(metadata->children);
    }
    if (metadata->next != NULL) {
        freeMetadata(metadata->next);
    }
    free(metadata);

    return;
}

DopplerMeasurement*
init_DopplerMeasurement()
{
    DopplerMeasurement* dm = ((DopplerMeasurement*)
                              malloc(sizeof(DopplerMeasurement)));

    if (dm == NULL) {
        puts("Memory allocation for DopplerMeasurement failed");
        return NULL;
    }

    dm->singles = (SingleSpectrum*)malloc(sizeof(SingleSpectrum));

    if (dm->singles == NULL) {
        puts("Memory allocation for SingleSpectrum failed");
        free(dm);
        return NULL;
    }

    dm->coinc = (CoincidenceSpectrum*)malloc(sizeof(CoincidenceSpectrum));

    if (dm->coinc == NULL) {
        puts("Memory allocation for CoincidenceSpectrum failed");
        free(dm->singles);
        free(dm);
        return NULL;
    }

    dm->n_singles = 0;
    dm->n_coinc = 0;

    return dm;
}

static void
spectrumStrToArr(char* spectrum, SingleSpectrum* s)
{
    if (spectrum == NULL) {
        return;
    }

    s->spectrum_size = -1;

    int length = strlen(spectrum);
    s->spectrum = (int*)malloc(length * sizeof(int));

    if (s->spectrum == NULL) {
        puts("Memory allocation for SingleSpectrum array failed");
        return;
    }

    int number;
    char* ptr = spectrum;
    char* space = " ";

    while (sscanf(ptr, "%d", &number) == 1) {
        s->spectrum[s->spectrum_size++] = number;
        do {
            ptr++;
        } while (*ptr != *space && *ptr);
    }
    s->spectrum_size++;

    s->spectrum = (int*)realloc(s->spectrum, s->spectrum_size * sizeof(int));

    return;
}

static DopplerMeasurement*
metadataToDoppler(metadata_item* metadata, const char* filename)
{
    if (metadata == NULL) {
        return NULL;
    }

    DopplerMeasurement* dm = init_DopplerMeasurement();

    if (dm == NULL) {
        return NULL;
    }

    dm->metadata = metadata;

    metadata_item* data = getItem(metadata, "RadMeasurement");

    if (data == NULL) {
        printf("%s: No measurement data found\n", filename);
        return dm;
    }

    for (metadata_item* entry = data->children;
            entry != NULL; entry = entry->next) {
        if (!strcmp(entry->key, "Spectrum")) {
            metadata_item* id = getItem(entry, "id");
            if (strstr(id->value, "Coinc") == NULL) {
                int i = dm->n_singles;
                if (i > 0) {
                    void* new_singles = realloc(dm->singles,
                                            (i + 1) * sizeof(SingleSpectrum));
                    if (new_singles == NULL) {
                        printf("%s: Could not extend SingleSpectrum array\n",
                               filename);
                        continue;
                    }
                    dm->singles = (SingleSpectrum*)new_singles;
                }
                dm->n_singles++;
                dm->singles[i].filename = strdup(filename);
                for (metadata_item* attr = entry->children;
                        attr != NULL; attr = attr->next) {
                    if (!strcmp(attr->key, "ChannelData")) {
                        spectrumStrToArr(attr->value, &(dm->singles[i]));
                    } else if (!strcmp(attr->key,
                                "radDetectorInformationReference")) {
                        dm->singles[i].detname.ref = strdup(attr->value);
                    } else if (!strcmp(attr->key,
                                "energyCalibrationReference")) {
                        dm->singles[i].ecal.ref = strdup(attr->value);
                    }
                }
            } else {
                // TODO
            }
        }
    }

    return dm;
}

void
printDopplerMeasurement(const DopplerMeasurement* dm)
{
    if (dm->singles != NULL) {
        printf("Doppler Measurement %s:\n", dm->singles[0].filename);
        printf("DopplerMeasurement contains %d Single ", dm->n_singles);
        switch (dm->n_singles) {
            case 0:
                puts("Spectra");
                break;
            case 1:
                puts("Spectrum:");
                break;
            default:
                puts("Spectra:");
                break;
        }
        for (int i = 0; i < dm->n_singles; i++) {
            int l = dm->singles[i].spectrum_size;
            printf("Spectrum %d (length %d):", i, l);
            for (int j = 0; j < l; j++) {
                printf(" %d", dm->singles[i].spectrum[j]);
            }
            puts("");
            printf("Filename: %s\n", dm->singles[i].filename);
            printf("Detector: %s\n", dm->singles[i].detname.ref);
            printf("Ecal: %s\n", dm->singles[i].ecal.ref);
        }
    }

    if (dm->coinc != NULL) {
        printf("Doppler Measurement contains %d Coincidence Spectra:\n",
               dm->n_coinc);
        for (int i = 0; i < dm->n_coinc; i++) {
            printf("%s", dm->coinc[0].filename);
        }
    }

    return;
}

DopplerMeasurement*
import_n42(const char* filepath, const int verbose)
{
	xmlDocPtr doc = xmlParseFile(filepath);

	if (doc == NULL) {
		fprintf(stderr, "Document not parsed successfully.\n");
		return NULL;
	}

    xmlNodePtr root = xmlDocGetRootElement(doc);

    metadata_item* metadata = loadxmlTree(root);

    xmlFreeDoc(doc);
    xmlCleanupParser();

    if (verbose) {
        printMetadata(metadata);
    }

    DopplerMeasurement* dm = metadataToDoppler(metadata, filepath);

    if (verbose) {
        printDopplerMeasurement(dm);
    }

    return dm;
}

void
freeDopplerMeasurement(DopplerMeasurement* dm)
{
    if (dm != NULL) {
        if (dm->metadata != NULL) {
            freeMetadata(dm->metadata);
        }
        if (dm->singles != NULL) {
            for (int i = 0; i < dm->n_singles; i++) {
                if (dm->singles[i].spectrum != NULL) {
                    free(dm->singles[i].spectrum);
                }
                if (dm->singles[i].filename != NULL) {
                    free(dm->singles[i].filename);
                }
                if (dm->singles[i].detname.ref != NULL) {
                    free(dm->singles[i].detname.ref);
                }
                if (dm->singles[i].ecal.ref != NULL) {
                    free(dm->singles[i].ecal.ref);
                }
            }
            free(dm->singles);
        }
        if (dm->coinc != NULL) {
            for (int i = 0; i < dm->n_coinc; i++) {
                free(dm->coinc[i].spectrum);
            }
            free(dm->coinc);
        }
        free(dm);
    }

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
    char* filepath = ("./example.n42");

    DopplerMeasurement* dm = import_n42(filepath, verbose);

    freeDopplerMeasurement(dm);

    stop = clock();
    cpu_time = 1000 * ((double)(stop - start)) / CLOCKS_PER_SEC;

    printf("CPU time used: %f ms\n", cpu_time);

    return 0;
}


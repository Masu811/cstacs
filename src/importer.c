#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <libxml/parser.h>
#include <libxml/tree.h>

#define STB_IMAGE_IMPLEMENTATION
#include "../include/stb_image.h"

#include "../include/importer.h"
#include "../include/structs.h"

/*
 * Get the first child of metadata item 'node' whose key equals 'key'.
 * If found, return the pointer to that child, else return NULL.
 */
static metadata_item *getItem(metadata_item *node, char *key) {
    if (node == NULL || node->children == NULL) return NULL;

    for (metadata_item *child = node->children; child; child = child->next) {
        if (strcmp(child->key, key) == 0) return child;
    }

    return NULL;
}

/*
 * Check if xmlNode 'node' has only children of type XML_TEXT_NODE.
 * If so, return 1, else return 0.
 */
static int onlyTextChildren(xmlNodePtr node) {
    if (node == NULL || node->children == NULL) return 0;

    for (xmlNodePtr child = node->children; child; child = child->next) {
        if (child->type != XML_TEXT_NODE) {
            return 0;
        }
    }

    return 1;
}

static metadata_item *loadxmlTree(xmlNodePtr root);

/*
 * Insert xml attribute nodes as metadata item in front of 'next'.
 * Create linked list of metadata items representing the attribute nodes.
 * 'next' will be the next of the last attribute.
 */
static metadata_item *insertAttr(xmlAttrPtr attr, xmlNodePtr next) {
    if (attr == NULL) return NULL;

    metadata_item *metadata = (metadata_item *)malloc(sizeof(metadata_item));

    if (metadata == NULL) {
        puts("Memory allocation for metadata failed");
        return NULL;
    }

    metadata->key = strdup((char *)attr->name);
    metadata->value = strdup((char *)attr->children->content);
    metadata->children = NULL;

    if (attr->next == NULL) {
        metadata->next = loadxmlTree(next);
    } else {
        metadata->next = insertAttr(attr->next, next);
    }

    return metadata;
}

/*
 * Recreate the xml tree branching off of 'root' as metadata item linked list.
 */
static metadata_item *loadxmlTree(xmlNodePtr root) {
    if (root == NULL) {
        return NULL;
    } else if (root->type == XML_TEXT_NODE) {
        return loadxmlTree(root->next);
    }

    metadata_item *metadata = (metadata_item *)malloc(sizeof(metadata_item));

    if (metadata == NULL) {
        puts("Memory allocation for metadata failed");
        return NULL;
    }

    if (root->name != NULL) {
        metadata->key = strdup((char *)root->name);
    } else {
        metadata->key = NULL;
    }

    if (root->properties != NULL) {
        metadata->children = insertAttr(root->properties, root->children);
    } else if (root->children == NULL) {
        metadata->children = NULL;
    } else {
        metadata->children = loadxmlTree(root->children);
    }

    metadata->value = NULL;

    if (onlyTextChildren(root)) {
        metadata->value = strdup((char *)root->children->content);
    }

    metadata->next = loadxmlTree(root->next);

    return metadata;
}

/*
 * Traverse the metadata tree 'metadata' and print keys and values.
 */
static void printMetadata(metadata_item *metadata) {
    if (metadata == NULL) return;

    printf("%s: ", metadata->key);

    if (metadata->value != NULL) {
        printf("%s\n", metadata->value);
    } else {
        printf("\n");
    }

    for (metadata_item *child = metadata->children; child;
         child = child->next) {
        printMetadata(child);
    }
}

static void freeMetadata(metadata_item *metadata) {
    if (metadata == NULL) return;

    if (metadata->key != NULL) {
        free(metadata->key);
        metadata->key = NULL;
    }
    if (metadata->value != NULL) {
        free(metadata->value);
        metadata->value = NULL;
    }

    if (metadata->children != NULL) freeMetadata(metadata->children);

    if (metadata->next != NULL) freeMetadata(metadata->next);

    free(metadata);
    metadata = NULL;
}

/*
 * Load string 'spectrum' into int array 's->spectrum'.
 * 'spectrum' is expected to be a string of ints separated by spaces.
 */
static void spectrumStrToArr(char *spectrum, SingleSpectrum *s, bool debug) {
    if (spectrum == NULL || s == NULL) return;

    s->spectrum_size = 0;

    int length = strlen(spectrum) / 2;
    s->spectrum = (int *)malloc(length * sizeof(int));
    if (debug) printf("Allocating Single Spectrum buffer of size %d\n", length);

    if (s->spectrum == NULL) {
        puts("Memory allocation for SingleSpectrum array failed");
        return;
    }

    int number;
    char *ptr = spectrum;
    char *endptr;

    while (ptr != NULL) {
        number = (int)strtol(ptr, &endptr, 10);
        if (ptr == endptr) break;
        s->spectrum[s->spectrum_size++] = number;
        ptr = endptr;
    }

    if (debug) {
        printf("Downsizing Single Spectrum buffer to %d\n", s->spectrum_size);
    }

    s->spectrum = (int *)realloc(s->spectrum, s->spectrum_size * sizeof(int));
}

/*
 * Load png file 'filename' into int array 'coinc->spectrum'.
 */
static void import_png(char *filename, CoincidenceSpectrum *coinc) {
    int width, height, channels;
    unsigned char *img = stbi_load(filename, &width, &height, &channels, 3);

    if (img == NULL) {
        printf("Error: Could not load png file %s\n", filename);
        return;
    }

    coinc->width = width;
    coinc->height = height;
    coinc->spectrum = (int *)malloc(sizeof(int) * width * height);
    if (coinc->spectrum == NULL) {
        perror("Failed to allocate memory for coincidence spectrum");
        stbi_image_free(img);
        return;
    }

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int index = (y * width + x) * 3;
            int value =
                img[index] | (img[index + 1] << 8) | (img[index + 2] << 16);
            coinc->spectrum[y * width + x] = value;
        }
    }

    stbi_image_free(img);
}

/*
 * Concatenate 'directory' and 'filename'.
 * 'directory' must end in '/'.
 * The user is responsible for freeing the returned char*.
 */
char *concatPath(char *directory, char *filename) {
    int l = strlen(directory) + strlen(filename) + 1;
    char *filepath = (char *)malloc(l);
    snprintf(filepath, l, "%s%s", directory, filename);

    return filepath;
}

/* Assemble DopplerMeasurement from metadata. */
static DopplerMeasurement *metadataToDoppler(
    metadata_item *metadata, char *directory, char *filename, bool debug
) {
    if (metadata == NULL) return NULL;

    DopplerMeasurement *dm =
        ((DopplerMeasurement *)calloc(1, sizeof(DopplerMeasurement)));

    if (dm == NULL) return NULL;

    dm->n_singles = 0;
    dm->n_coinc = 0;
    dm->metadata = metadata;

    metadata_item *data = getItem(metadata, "RadMeasurement");

    if (data == NULL) {
        printf("%s: No measurement data found\n", filename);
        return dm;
    }

    for (metadata_item *entry = data->children; entry; entry = entry->next) {
        if (strcmp(entry->key, "Spectrum") == 0) {
            metadata_item *id = getItem(entry, "id");
            if (strstr(id->value, "Coinc") == 0) {
                int i = dm->n_singles;
                SingleSpectrum *new_single =
                    (SingleSpectrum *)calloc(1, sizeof(SingleSpectrum));

                if (new_single == NULL) {
                    printf(
                        "%s: Could not create new SingleSpectrum\n", filename
                    );
                    continue;
                }

                dm->singles = ((SingleSpectrum **)realloc(
                    dm->singles, (i + 1) * sizeof(SingleSpectrum *)
                ));
                dm->singles[i] = new_single;
                dm->n_singles++;
                dm->singles[i]->filename = strdup(filename);

                for (metadata_item *attr = entry->children; attr;
                     attr = attr->next) {
                    if (strcmp(attr->key, "ChannelData") == 0) {
                        spectrumStrToArr(attr->value, dm->singles[i], debug);
                    } else if (strcmp(
                                   attr->key, "radDetectorInformationReference"
                               ) == 0) {
                        dm->singles[i]->detname = strdup(attr->value);
                    } else if (strcmp(
                                   attr->key, "energyCalibrationReference"
                               ) == 0) {
                        dm->singles[i]->ecal.ref = strdup(attr->value);
                    }
                }
            } else {
                int i = dm->n_coinc;
                CoincidenceSpectrum *new_coinc = ((CoincidenceSpectrum *)calloc(
                    1, sizeof(CoincidenceSpectrum)
                ));

                if (new_coinc == NULL) {
                    printf(
                        "%s: Could not create new CoincidenceSpectrum\n",
                        filename
                    );
                    continue;
                }

                dm->coinc = ((CoincidenceSpectrum **)realloc(
                    dm->coinc, (i + 1) * sizeof(CoincidenceSpectrum *)
                ));
                dm->coinc[i] = new_coinc;
                dm->n_coinc++;
                dm->coinc[i]->parentname = strdup(filename);
                char *filepath = concatPath(directory, entry->value);
                dm->coinc[i]->filename = strdup(filepath);

                import_png(filepath, dm->coinc[i]);
                free(filepath);

                for (metadata_item *attr = entry->children; attr;
                     attr = attr->next) {
                    if (strcmp(attr->key, "radDetectorInformationReference") ==
                        0) {
                        dm->coinc[i]->detpair = strdup(attr->value);
                        break;
                    }
                }
            }
        }
    }

    // resolve references

    for (int i = 0; i < dm->n_singles; i++) {
        for (metadata_item *entry = metadata->children; entry;
             entry = entry->next) {
            if (strcmp(entry->key, "RadDetectorInformation") == 0) {
                metadata_item *id = getItem(entry, "id");

                if (id == NULL) continue;
                if (strcmp(id->value, dm->singles[i]->detname) != 0) continue;

                metadata_item *name = getItem(entry, "RadDetectorName");

                free(dm->singles[i]->detname);

                dm->singles[i]->detname = strdup(name->value);
            } else if (strcmp(entry->key, "EnergyCalibration") == 0) {
                metadata_item *id = getItem(entry, "id");

                if (id == NULL) continue;
                if (dm->singles[i]->ecal_found == true) continue;
                if (strcmp(id->value, dm->singles[i]->ecal.ref) != 0) continue;

                metadata_item *ecal = getItem(entry, "CoefficientValues");

                free(dm->singles[i]->ecal.ref);
                dm->singles[i]->ecal.ref = NULL;

                char *ptr = ecal->value, *endptr = ecal->value;

                for (int j = 0; j < 3; j++) {
                    if (*ptr == '\0') break;
                    dm->singles[i]->ecal.values[j] = strtof(ptr, &endptr);
                    if (ptr == endptr) {
                        puts("Energy calibration missing or malformed");
                        break;
                    }
                    ptr = endptr;
                }
                dm->singles[i]->ecal_found = true;
            }
        }
    }

    return dm;
}

void printDopplerMeasurement(DopplerMeasurement *dm) {
    if (dm == NULL) return;

    if (dm->singles != NULL) {
        printf("Doppler Measurement %s:\n", (*dm->singles)[0].filename);
        printf("\tDopplerMeasurement contains %d Single ", dm->n_singles);
        if (dm->n_singles == 1) {
            puts("Spectrum:");
        } else {
            puts("Spectra:");
        }

        for (int i = 0; i < dm->n_singles; i++) {
            printf("\t\tSpectrum %d:\n", i);
            printf("\t\t\tChannels: %d\n", dm->singles[i]->spectrum_size);
            printf("\t\t\tFilename: %s\n", dm->singles[i]->filename);
            printf("\t\t\tDetector: %s\n", dm->singles[i]->detname);
            if (dm->singles[i]->ecal_found) {
                printf(
                    "\t\t\tEcal: (%f, %f)\n", dm->singles[i]->ecal.values[0],
                    dm->singles[i]->ecal.values[1]
                );
            }
            printf("\t\t\tCounts: %lld\n", dm->singles[i]->counts);
            printf("\t\t\tS: %f +/- %f\n", dm->singles[i]->s, dm->singles[i]->ds);
            printf("\t\t\tW: %f +/- %f\n", dm->singles[i]->w, dm->singles[i]->dw);
            printf("\t\t\tV/P: %f +/- %f\n", dm->singles[i]->v2p, dm->singles[i]->dv2p);
        }
    }

    if (dm->coinc != NULL) {
        printf("\tDoppler Measurement contains %d Coincidence ", dm->n_coinc);
        if (dm->n_coinc == 1) {
            puts("Spectrum:");
        } else {
            puts("Spectra:");
        }

        for (int i = 0; i < dm->n_coinc; i++) {
            printf("\t\tSpectrum %d:\n", i);
            printf(
                "\t\t\tWidth (Detector 1 Channels): %d\n", dm->coinc[i]->width
            );
            printf(
                "\t\t\tHeight (Detector 2 Channels): %d\n", dm->coinc[i]->height
            );
            printf("\t\t\tFilename: %s\n", dm->coinc[i]->filename);
            printf("\t\t\tParentname: %s\n", dm->coinc[i]->parentname);
            printf("\t\t\tDetectorpair: %s\n", dm->coinc[i]->detpair);
            printf("\t\t\tCounts: %lld\n", dm->coinc[i]->counts);
        }
    }
}

/*
 * Assemble DopplerMeasurement from n42 file.
 */
DopplerMeasurement *import_n42(char *directory, char *filename, bool debug) {
    char *filepath = concatPath(directory, filename);
    xmlDocPtr doc = xmlParseFile(filepath);
    free(filepath);

    if (doc == NULL) {
        printf("Document not parsed successfully. Filepath might be wrong.\n");
        return NULL;
    }

    xmlNodePtr root = xmlDocGetRootElement(doc);

    metadata_item *metadata = loadxmlTree(root);

    xmlFreeDoc(doc);
    xmlCleanupParser();

    DopplerMeasurement *dm = metadataToDoppler(
        metadata, directory, filename, debug
    );

    if (debug) printDopplerMeasurement(dm);

    return dm;
}

void freeDopplerMeasurement(DopplerMeasurement *dm) {
    if (dm == NULL) return;

    if (dm->metadata != NULL) {
        freeMetadata(dm->metadata);
        dm->metadata = NULL;
    }

    if (dm->singles != NULL) {
        for (int i = 0; i < dm->n_singles; i++) {
            if (dm->singles[i] == NULL) continue;

            if (dm->singles[i]->spectrum != NULL) {
                free(dm->singles[i]->spectrum);
                dm->singles[i]->spectrum = NULL;
            }
            if (dm->singles[i]->energies != NULL) {
                free(dm->singles[i]->energies);
                dm->singles[i]->energies = NULL;
            }
            if (dm->singles[i]->filename != NULL) {
                free(dm->singles[i]->filename);
                dm->singles[i]->filename = NULL;
            }
            if (dm->singles[i]->detname != NULL) {
                free(dm->singles[i]->detname);
                dm->singles[i]->detname = NULL;
            }
            free(dm->singles[i]);
            dm->singles[i] = NULL;
        }
        free(dm->singles);
        dm->singles = NULL;
    }

    if (dm->coinc != NULL) {
        for (int i = 0; i < dm->n_coinc; i++) {
            if (dm->coinc[i] == NULL) continue;

            if (dm->coinc[i]->spectrum) {
                free(dm->coinc[i]->spectrum);
                dm->coinc[i]->spectrum = NULL;
            }
            if (dm->coinc[i]->filename) {
                free(dm->coinc[i]->filename);
                dm->coinc[i]->filename = NULL;
            }
            if (dm->coinc[i]->parentname) {
                free(dm->coinc[i]->parentname);
                dm->coinc[i]->parentname = NULL;
            }
            if (dm->coinc[i]->detpair) {
                free(dm->coinc[i]->detpair);
                dm->coinc[i]->detpair = NULL;
            }
            free(dm->coinc[i]);
            dm->coinc[i] = NULL;
        }
        free(dm->coinc);
        dm->coinc = NULL;
    }

    free(dm);
    dm = NULL;
}

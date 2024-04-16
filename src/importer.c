#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <libxml/parser.h>
#include <libxml/tree.h>
#include <png.h>

#include "importer.h"
#include "structs.h"

/*
 * Get the first child of metadata item 'metadata' whose key equals 'key'.
 * If found, return the metadata_item* to that child, else return NULL.
 */
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

/*
 * Check if xmlNode '*node' has only children of type XML_TEXT_NODE.
 * If so, return 1, else return 0.
 */
static int
onlyTextChildren(const xmlNodePtr node)
{
    if (node->children == NULL) {
        return 0;
    }

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

/*
 * Insert xml attribute nodes as metadata item in front of 'next'.
 * Create linked list of metadata items representing the attribute nodes.
 * 'next' will be the next of the last attribute.
 */
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

    metadata->key = strdup((char*)attr->name);
    metadata->value = strdup((char*)attr->children->content);
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
        metadata->key = strdup((char*)root->name);
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

    if (onlyTextChildren(root)){
        metadata->value = strdup((char*)root->children->content);
    }

    metadata->next = loadxmlTree(root->next);

    return metadata;
}

/*
 * Traverse the metadata tree 'metadata' and print keys and values.
 */
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

    for (metadata_item* child = metadata->children;
            child != NULL; child = child->next) {
        printMetadata(child);
    }

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

/*
 * Load char* 'spectrum' into int array 's->spectrum'.
 * 'spectrum' is expected to be a string of ints separated by spaces.
 */
static void
spectrumStrToArr(char* spectrum, SingleSpectrum* s)
{
    if (spectrum == NULL) {
        return;
    }

    s->spectrum_size = 0;

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

    s->spectrum = (int*)realloc(s->spectrum, s->spectrum_size * sizeof(int));

    return;
}

/*
 * Load png file 'filename' into int array 'coinc->spectrum'.
 */
static void
import_png(char* filename, CoincidenceSpectrum* coinc)
{
    int width, height;
    png_byte color_type;
    png_byte bit_depth;
    png_bytep *row_pointers = NULL;
    FILE *fp = fopen(filename, "rb");

    png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING,
                                             NULL, NULL, NULL);
    if(!png) abort();

    png_infop info = png_create_info_struct(png);
    if(!info) abort();

    if(setjmp(png_jmpbuf(png))) abort();

    png_init_io(png, fp);

    png_read_info(png, info);

    width      = png_get_image_width(png, info);
    height     = png_get_image_height(png, info);
    color_type = png_get_color_type(png, info);
    bit_depth  = png_get_bit_depth(png, info);

    if (color_type != PNG_COLOR_TYPE_RGB) {
        puts("Currently only RGB PNG evaluation is implemented");
        fclose(fp);
        png_destroy_read_struct(&png, &info, NULL);
        return;
    }

    png_read_update_info(png, info);

    if (row_pointers) abort();

    row_pointers = (png_bytep*)malloc(sizeof(png_bytep) * height);
    for(int y = 0; y < height; y++) {
        row_pointers[y] = (png_byte*)malloc(png_get_rowbytes(png,info));
    }

    png_read_image(png, row_pointers);

    fclose(fp);

    png_destroy_read_struct(&png, &info, NULL);

    coinc->spectrum = (int*)malloc(sizeof(int) * width * height);
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            png_byte* ptr = &(row_pointers[y][x * 3]);
            int value = ptr[0] | (ptr[1] << 8) | (ptr[2] << 16);
            coinc->spectrum[y * width + x] = value;
        }
    }

    for(int y = 0; y < height; y++) {
        free(row_pointers[y]);
    }
    free(row_pointers);

    coinc->width = width;
    coinc->height = height;

    return;
}

/* 
 * Concatenate 'directory' and 'filename'.
 * 'directory' must end in '/'.
 * The user is responsible for freeing the returned char*.
 */
char*
concatPath(const char* directory, const char* filename)
{
    int l = strlen(directory) + strlen(filename) + 1;
    char* filepath = (char*)malloc(l);
    snprintf(filepath, l, "%s%s", directory, filename);

    return filepath;
}

/* Assemble DopplerMeasurement from metadata. */
static DopplerMeasurement*
metadataToDoppler(metadata_item* metadata, const char* directory,
                  const char* filename)
{
    if (metadata == NULL) {
        return NULL;
    }

    DopplerMeasurement* dm = ((DopplerMeasurement*)
                              calloc(1, sizeof(DopplerMeasurement)));

    if (dm == NULL) {
        return NULL;
    }

    dm->n_singles = 0;
    dm->n_coinc = 0;
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
                SingleSpectrum* new_single = ((SingleSpectrum*)
                                              calloc(1,
                                                     sizeof(SingleSpectrum)));
                if (new_single == NULL) {
                    printf("%s: Could not create new SingleSpectrum\n",
                           filename);
                    continue;
                }
                dm->singles = ((SingleSpectrum**)
                               realloc(dm->singles,
                                       (i + 1) * sizeof(SingleSpectrum*)));
                dm->singles[i] = new_single;
                dm->n_singles++;
                dm->singles[i]->filename = strdup(filename);
                for (metadata_item* attr = entry->children;
                        attr != NULL; attr = attr->next) {
                    if (!strcmp(attr->key, "ChannelData")) {
                        spectrumStrToArr(attr->value, dm->singles[i]);
                    } else if (!strcmp(attr->key,
                                "radDetectorInformationReference")) {
                        dm->singles[i]->detname.ref = strdup(attr->value);
                    } else if (!strcmp(attr->key,
                                "energyCalibrationReference")) {
                        dm->singles[i]->ecal.ref = strdup(attr->value);
                    }
                }
            } else {
                int i = dm->n_coinc;
                CoincidenceSpectrum* new_coinc = ((CoincidenceSpectrum*)
                        calloc(1, sizeof(CoincidenceSpectrum)));
                if (new_coinc == NULL) {
                    printf("%s: Could not create new CoincidenceSpectrum\n",
                           filename);
                    continue;
                }
                dm->coinc = ((CoincidenceSpectrum**)
                             realloc(dm->coinc,
                                     (i + 1) * sizeof(CoincidenceSpectrum*)));
                dm->coinc[i] = new_coinc;
                dm->n_coinc++;
                dm->coinc[i]->parentname = strdup(filename);
                char* filepath = concatPath(directory, entry->value);
                dm->coinc[i]->filename = strdup(filepath);
                import_png(filepath, dm->coinc[i]);
                free(filepath);
                for (metadata_item* attr = entry->children;
                        attr != NULL; attr = attr->next) {
                    if (!strcmp(attr->key,
                                "radDetectorInformationReference")) {
                        dm->coinc[i]->detpair.ref = strdup(attr->value);
                    }
                }
            }
        }
    }

    // resolve references

    for (int i = 0; i < dm->n_singles; i++) {
        for (metadata_item* entry = metadata->children;
                entry != NULL; entry = entry->next) {
            if (!strcmp(entry->key, "RadDetectorInformation")) {
                metadata_item* id = getItem(entry, "id");
                if (id == NULL) {
                    continue;
                }
                if (strcmp(id->value, dm->singles[i]->detname.ref)) {
                    continue;
                }
                metadata_item* name = getItem(entry, "RadDetectorName");
                free(dm->singles[i]->detname.ref);
                dm->singles[i]->detname.name = strdup(name->value);
            } else if (!strcmp(entry->key, "EnergyCalibration")) {
                metadata_item* id = getItem(entry, "id");
                if (id == NULL) {
                    continue;
                }
                if (strcmp(id->value, dm->singles[i]->ecal.ref)) {
                    continue;
                }
                metadata_item* ecal = getItem(entry, "CoefficientValues");
                free(dm->singles[i]->ecal.ref);
                dm->singles[i]->ecal.values = strdup(ecal->value);
            }
        }
    }

    return dm;
}

void
printDopplerMeasurement(const DopplerMeasurement* dm)
{
    if (dm->singles != NULL) {
        printf("Doppler Measurement %s:\n", (*dm->singles)[0].filename);
        printf("\tDopplerMeasurement contains %d Single ", dm->n_singles);
        switch (dm->n_singles) {
            case 1:
                puts("Spectrum:");
                break;
            default:
                puts("Spectra:");
                break;
        }
        for (int i = 0; i < dm->n_singles; i++) {
            printf("\t\tSpectrum %d:\n", i);
            printf("\t\t\tChannels: %d\n", dm->singles[i]->spectrum_size);
            printf("\t\t\tFilename: %s\n", dm->singles[i]->filename);
            printf("\t\t\tDetector: %s\n", dm->singles[i]->detname.ref);
            printf("\t\t\tEcal: %s\n", dm->singles[i]->ecal.ref);
            printf("\t\t\tCounts: %lld\n", dm->singles[i]->counts);
        }
    }

    if (dm->coinc != NULL) {
        printf("\tDoppler Measurement contains %d Coincidence ", dm->n_coinc);
        switch (dm->n_coinc) {
            case 1:
                puts("Spectrum:");
                break;
            default:
                puts("Spectra:");
                break;
        }
        for (int i = 0; i < dm->n_coinc; i++) {
            printf("\t\tSpectrum %d:\n", i);
            printf("\t\t\tWidth (Detector 1 Channels): %d\n",
                    dm->coinc[i]->width);
            printf("\t\t\tHeight (Detector 2 Channels): %d\n",
                    dm->coinc[i]->height);
            printf("\t\t\tFilename: %s\n", dm->coinc[i]->filename);
            printf("\t\t\tParentname: %s\n", dm->coinc[i]->parentname);
            printf("\t\t\tDetectorpair: %s\n", dm->coinc[i]->detpair.ref);
            printf("\t\t\tCounts: %lld\n", dm->coinc[i]->counts);
        }
    }

    return;
}

/*
 * Assemble DopplerMeasurement from n42 file.
 */
DopplerMeasurement*
import_n42(const char* directory, const char* filename, const int verbose)
{
    char* filepath = concatPath(directory, filename);
	xmlDocPtr doc = xmlParseFile(filepath);
    free(filepath);

	if (doc == NULL) {
		fprintf(stderr, "Document not parsed successfully.\n");
		return NULL;
	}

    xmlNodePtr root = xmlDocGetRootElement(doc);

    metadata_item* metadata = loadxmlTree(root);

    xmlFreeDoc(doc);
    xmlCleanupParser();
    /*
    if (verbose) {
        printf("\n%s:\n", filepath);
        printf("\n%s: Extracted the following metadata:\n", filepath);
        printMetadata(metadata);
        printf("\n%s: End of metadata\n\n", filepath);
    }
    */
    DopplerMeasurement* dm = metadataToDoppler(metadata, directory, filename);

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
                if (dm->singles[i]->spectrum != NULL) {
                    free(dm->singles[i]->spectrum);
                }
                if (dm->singles[i]->filename != NULL) {
                    free(dm->singles[i]->filename);
                }
                if (dm->singles[i]->detname.ref != NULL) {
                    free(dm->singles[i]->detname.name);
                }
                if (dm->singles[i]->ecal.ref != NULL) {
                    free(dm->singles[i]->ecal.values);
                }
                if (dm->singles[i] != NULL) {
                    free(dm->singles[i]);
                }
            }
            free(dm->singles);
        }
        if (dm->coinc != NULL) {
            for (int i = 0; i < dm->n_coinc; i++) {
                if (dm->coinc[i]->spectrum != NULL) {
                    free(dm->coinc[i]->spectrum);
                }
                if (dm->coinc[i]->filename != NULL) {
                    free(dm->coinc[i]->filename);
                }
                if (dm->coinc[i]->parentname != NULL) {
                    free(dm->coinc[i]->parentname);
                }
                if (dm->coinc[i]->detpair.ref != NULL) {
                    free(dm->coinc[i]->detpair.ref);
                }
                if (dm->coinc[i] != NULL) {
                    free(dm->coinc[i]);
                }
            }
            free(dm->coinc);
        }
        free(dm);
    }

    return;
}


/*
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
    char* filepath = ("example.n42");

    DopplerMeasurement* dm = import_n42("./", filepath, verbose);

    freeDopplerMeasurement(dm);

    stop = clock();
    cpu_time = 1000 * ((double)(stop - start)) / CLOCKS_PER_SEC;

    printf("CPU time used: %f ms\n", cpu_time);

    return 0;
}
*/

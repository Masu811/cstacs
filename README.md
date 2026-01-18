# CSTACS


## Description
CSTACS is a project that includes the re-implementation of the Python project [STACS](https://gitlab.lrz.de/tum-frm2-positrons/stacs) in C.

The current plan is to write the core functionality in C and then add both a GUI and other language bindings on top.

The sole purpose of this project is fun and learning. If the results are at some point in time any useful to anyone, great.

## Project Status
- [x] .n42 file import
  - [x] Datatypes
  - [x] Import of metadata
  - [x] .png file import
  - [x] Spectra import & type conversion
  - [x] Reference resolution (energy calibration, detector names)
- [ ] Spectra analysis
  - [ ] Single Spectra
    - [x] Peak parameters via Gaussian fit
    - [x] Peak extraction
    - [ ] Background subtraction
    - [ ] Line shape parameter calculation
  - [ ] Coincidence Spectra
    - [ ] Peak parameters via 2D-Gaussian fit
    - [ ] Background subtraction
    - [ ] Peak extraction
    - [ ] Line shape parameter calculation
- [ ] Higher level analysis
  - [ ] (Depth) profiles
  - [ ] Ratio curves

## Dependencies

Debian, Ubuntu
```
sudo apt install libxml2-dev libgsl-dev
```

Arch
```
sudo pacman -Sy libxml2 gsl
```

## Installation

To install the STACS shared library on your (Linux) system, run
```
make
```
while in the CSTACS root directory. This will install the `stacs.h` file in `/usr/include` and the `libstacs.so` file in `/usr/lib`.

To remove all installed files, run
```
make uninstall
```

## Basic Usage

Assume we have a folder `measurement` filled with `.n42` files and corresponding `.png` files. A basic scipt, let's call it `eval.c`, to analyze the data might look like this
```
#include <stacs/stacs.h>

extern int verbose;
extern int debug;

int main()
{
    verbose = 1;
    debug = 0;

    MeasurementCampaign *mc = importMeasurementCampaign("measurement/");
    printMeasurementCampaign(mc);
    evaluateMeasurementCampaign(mc);
    freeMeasurementCampaign(mc);

    return 0;
}
```
Here `importMeasurementCampaign` imports the data from the folder into RAM and computes things like total counts and energy of channels.

`printMeasurementCampaign` prints a summary of information about each Doppler measurement, namely filename and how many single and coincidence spectra it contains, and for each of those things like number of channels (per axis), filename, detector (pair) name, energy calibration and counts.

`evaluateMeasurementCampaign` fits theory functions to the peaks and calculates line-shape parameters (currently only Gaussian fit to single spectra).

`freeMeasurementCampaign` gives the occupied memory back to the system.

The flags `verbose` and `debug` can be used to get additional information during evaluation like peak parameters (verbose) or fit results (debug). Be careful though with larger measurement campaigns, as the output quickly becomes too much.

To compile this script, we need to link the STACS library with `-lstacs`
```
gcc -o eval eval.c -lstacs
```

## Unit Tests

### Dependencies

Debian / Ubuntu
```
sudo apt install libcriterion-dev
```

Arch
```
sudo pacman -Sy criterion
```

### Writing Tests

Test source files are placed in the `/tests` folder. Any `.c` file will be included in the make process.

### Running Tests

While in the project root, type
```
make test
```
which will compile and run the unit tests.

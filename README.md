# CSTACS


## Description
CSTACS is the C version of the Python project [STACS](https://gitlab.lrz.de/tum-frm2-positrons/stacs) with the goal of reducing computation time.

This project is not primarily intended as a replacement for the Python version. Instead, its purpose is to show which parts of the Python library could benefit significantly in single-thread performance from a C extension, following the example of NumPy.

Mentioning C extensions, CSTACS might also help with the integration of NVIDIA CUDA, which is planned for coincidence data evaluation in the future for both C and Python version.

Since CSTACS is (going to be) a fully functional twin of its Python counterpart, it might be used as an alternative altogether. The benefits of this version include performance maximization and RAM usage minimization, making the evaluation of e.g. large coincidence pngs or measurement campaigns possible. For this purpose, a RAM saver mode could be implemented in the future, that e.g. only ever holds one spectrum in RAM at a time and uses the smallest possible data types to store the data.

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

`freeMeasurementCampaign` gives the occupied memory back to the system (google `memory leak` if you don't know what that why this is necessary).

The flags `verbose` and `debug` can be used to get additional information during evaluation like peak parameters (verbose) or fit results (debug). Be careful though with larger measurement campaigns, as the output quickly becomes too much.

To compile this script, we need to link the STACS library with `-lstacs`
```
gcc -o eval eval.c -lstacs
```
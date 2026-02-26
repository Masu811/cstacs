# CSTACS - C Version


## Description
Congrats, you found the C version of [STACS](https://gitlab.lrz.de/tum-frm2-positrons/stacs).

## Project Status
- [x] .n42 file import
  - [x] Datatypes
  - [x] Import of metadata
  - [x] .png file import
  - [x] Spectra import & type conversion
  - [x] Reference resolution (energy calibration, detector names)
- [ ] Spectra analysis
  - [x] Single Spectra
    - [x] Peak parameters via Gaussian fit
    - [x] Peak extraction
    - [x] Background subtraction
    - [x] Line shape parameter calculation
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

int main() {
    MeasurementCampaign *mc = importMeasurementCampaign("data/", 0);

    double v2p_bounds[4] = {400, 500, 506, 516};

    evaluateMeasurementCampaign(
        mc, 1.1, 1.0, 3.0, 0, 60.0, 0, 1, v2p_bounds, 1, 1, 1
    );
    printMeasurementCampaign(mc);
    freeMeasurementCampaign(mc);

    return 0;
}
```
Here `importMeasurementCampaign` imports the data from the folder into RAM and computes things like total counts and energy of channels.

`evaluateMeasurementCampaign` fits theory functions to the peaks and calculates line-shape parameters (currently only Gaussian fit to single spectra).

`printMeasurementCampaign` prints a summary of information about each Doppler measurement, namely filename and how many single and coincidence spectra it contains, and for each of those things like number of channels (per axis), filename, detector (pair) name, energy calibration and counts.

`freeMeasurementCampaign` gives the occupied memory back to the system.

To compile this script, we need to link the STACS library with `-lstacs`
```
gcc -o eval eval.c -lstacs
```

This example can be found in the `examples/` directory. While in `examples/`, type
```
make
```
to compile the example and
```
make run
```
to compile and run it.

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

While in `tests/`, type
```
make
```
to compile, and 
```
make test
```
to comile and run the unit tests.

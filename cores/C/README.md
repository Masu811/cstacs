# CSTACS - C Core

This folder contains the C version of the STACS core library.

## Dependencies

Debian, Ubuntu
```bash
sudo apt install libxml2-dev libgsl-dev
```

Arch
```bash
sudo pacman -Sy libxml2 gsl
```

## Installation

To install the STACS shared library on your (Linux) system, run
```bash
make
```
while in this directory. This will install the `stacs.h` file in `/usr/include/` and the `libstacs.so` file in `/usr/lib/`.

To remove all installed files, run
```bash
make uninstall
```

## Basic Usage

Assume we have a folder `measurement` filled with `.n42` files and corresponding `.png` files. A basic scipt, let's call it `eval.c`, to analyze the data might look like this
```c
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
Here `importMeasurementCampaign` imports the data from the folder and computes things like total counts and energy of channels.

`evaluateMeasurementCampaign` fits theory functions to the peaks and calculates line-shape parameters.

`printMeasurementCampaign` prints a summary of information about each `DopplerMeasurement`, namely filename and how many single and coincidence spectra it contains, and for each of those things like lineshape parameters, number of channels (per axis), filename, detector (pair) name, energy calibration and counts.

`freeMeasurementCampaign` frees the MeasurementCampaign and all its contents, from the bottom up.

To compile this script, we need to link the STACS library with `-lstacs`
```bash
gcc -o eval eval.c -lstacs
```

The example file above can be found in the `examples/` directory. While in `examples/`, type
```bash
make
```
to compile the example and
```bash
make run
```
to compile and run it.

## Unit Tests

!!! The CI pipeline is out of order due to the move from GitLab to GitHub. You can still run the tests locally, though.

### Dependencies

Debian / Ubuntu
```bash
sudo apt install libcriterion-dev
```

Arch
```bash
sudo pacman -Sy criterion
```

### Writing Tests

Test source files are placed in the `tests/` folder. Any `.c` file will be included in the make process.

### Running Tests

While in `tests/`, type
```bash
make
```
to compile, and 
```bash
make test
```
to compile and run the unit tests.

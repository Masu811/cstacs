# CSTACS - C++ Core

## Installation

To install the STACS shared library on your (Linux) system, run
```bash
make
```
while in the CSTACS root directory. This will install the `stacs.hpp` files in `/usr/include/` and the `libstacs.so` file in `/usr/lib/`.

To remove all installed files, run
```bash
make uninstall
```

## Basic Usage

Assume we have a folder `measurement/` filled with one `measurement_1.n42` file and corresponding `.png` file. We can import the file's spectra into a `DopplerMeasurement` with `Single`- and `CoincidenceSpectra` in the following way.
```c++
#include <iostream>
#include <stacs/stacs.hpp>

int main() {
    DopplerMeasurement m("./measurement/measurement_1.n42");

    std::cout << m.shape() << std::endl;

    return 0;
}
```
Creating an object of the `DopplerMeasurement` class can be done via passing a string, namely the path to the `.n42` file.

To compile this script, we need to link the STACS library with `-lstacs`
```bash
g++ -o eval eval.cpp -lstacs
```

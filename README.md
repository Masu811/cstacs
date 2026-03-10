<<<<<<< HEAD
<<<<<<< HEAD
# CSTACS - C Version


## Description
<<<<<<< HEAD
CSTACS is a project that includes the re-implementation of the Python project [STACS](https://gitlab.lrz.de/tum-frm2-positrons/stacs) in other programming languages.

Currently, there are three other versions: C, C++ and Rust. You may find them on respective branches.

The current plan for the branches is the following:
- The C version might at some point be turned into a C extension module for the STACS Python version.
- The C++ version might at some point become an alternative to the Python version.
- The Rust version is a promising candidate for creating a complete GUI version of STACS with.

The sole purpose of this project is fun and learning. If the results are at some point in time any useful to anyone, great.
=======
Congrats, you found the C version of [STACS](https://gitlab.lrz.de/tum-frm2-positrons/stacs).
>>>>>>> C_main
=======
# CSTACS - Rust Version


## Description
Congrats, you found the Rust version of [STACS](https://gitlab.lrz.de/tum-frm2-positrons/stacs).
>>>>>>> Rust_main

## Project Status
- C: N42 and PNG importers work, Single line shape parameters can be calculated
- C++: N42 and PNG importers work, Single peak extraction works
- Rust: N42 and PNG importers work
=======
# CSTACS - C++ Version


## Description
Congrats, you found the C++ version of [STACS](https://gitlab.lrz.de/tum-frm2-positrons/stacs).

## Project Status

- [ ] .n42 file import
  - [ ] Import of metadata
  - [x] Coincidence spectra import (from .png files)
  - [x] Single spectra import
  - [ ] Reference resolution (energy calibration, detector names)
- [ ] SingleSpectrum class
  - [x] Default constructor
  - [x] Peak parameters via Gaussian fit
  - [x] Peak extraction
  - [ ] Background subtraction
  - [ ] Line shape parameter calculation
- [ ] CoincidenceSpectrum class
  - [x] Default constructor
  - [ ] Peak parameters via 2D-Gaussian fit
  - [ ] Background subtraction
  - [ ] Peak extraction
  - [ ] Line shape parameter calculation
- [ ] DopplerMeasurement class
  - [x] Default constructor
- [ ] MeasurementCampaign class
  - [x] Default constructor
  - [ ] (Depth) profiles
  - [ ] Ratio curves

## Installation

<<<<<<< HEAD
To install the STACS shared library on your (Linux) system, run
```bash
make
```
while in the CSTACS root directory. This will install the `stacs.hpp` files in `/usr/include` and the `libstacs.so` file in `/usr/lib`.

To remove all installed files, run
```bash
make uninstall
=======
After downloading this Git Repo and having switched to the Rust branch, you can include CSTACS in your local projects by adding its absolute path to your `Cargo.toml` files
```
[dependencies]
cstacs = { path = "/path/to/cstacs" }
>>>>>>> Rust_main
```
This works on Windows accordingly.

## Basic Usage

<<<<<<< HEAD
Assume we have a folder `measurement` filled with one `measurement_1.n42` file and corresponding `.png` file. We can import the file's spectra into a `DopplerMeasurement` with `Single`- and `CoincidenceSpectra` in the following way.
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
>>>>>>> C++_main
=======
Assume we have a folder `measurements` filled with `.n42` files and corresponding `.png` files. A basic scipt, let's call it `eval.rs`, to analyze the data might look like this
```Rust
use cstacs::importer::n42_importer::import_n42;

fn main() -> anyhow::Result<()> {
    let m = import_n42("measurements/measurement_1.n42")?;

    println!("{}", m.shape());

    Ok(())
}
```

Here `import_n42` imports one `DopplerMeasurement`, named `m`. Then, we print the shape of `m` to see how many `SingleSpectra` and `CoincidenceSpectra` it contains.
>>>>>>> Rust_main

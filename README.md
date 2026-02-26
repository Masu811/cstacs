# CSTACS - Rust Version


## Description
Congrats, you found the Rust version of [STACS](https://gitlab.lrz.de/tum-frm2-positrons/stacs).

## Project Status
- [x] .n42 file import
  - [x] Datatypes
  - [x] Import of metadata
  - [x] .png file import
  - [x] Spectra import & type conversion
  - [x] Reference resolution (energy calibration, detector names)
- [ ] Spectra analysis
  - [ ] Single Spectra
    - [ ] Peak parameters via Gaussian fit
    - [ ] Peak extraction
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

After downloading this Git Repo and having switched to the Rust branch, you can include CSTACS in your local projects by adding its absolute path to your `Cargo.toml` files
```
[dependencies]
cstacs = { path = "/path/to/cstacs" }
```
This works on Windows accordingly.

## Basic Usage

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

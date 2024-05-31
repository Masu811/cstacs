# CSTACS


## Description
CSTACS is the C library and Python extension library of the Python project [STACS](https://gitlab.lrz.de/tum-frm2-positrons/stacs). As a Python extension library, CSTACS aims at optimizing performance critical parts of the Python library, following the example of NumPy.

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

## Authors and acknowledgment
Implementation of the original Python library:
- Vassily Burwitz
- Leon Chryssos
- Lucian Mathes

Translation to C:
- Maximilian Suhr

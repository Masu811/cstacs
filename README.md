# CSTACS – STACS in C


## Description
CSTACS is the implementation of the Python project [STACS](https://gitlab.lrz.de/tum-frm2-positrons/stacs) in the C programming language aiming to minimize computation time.

## Project status
Currently it's possible to create Doppler measurements from n42 / png files and measurement campaigns from a directory of such. Doppler measurements contain metadata and in particular single and coincidence spectra in the form of integer arrays. The total counts of spectra can be calculated as first parameter.

## Roadmap
1. **Recreate the basic functionality of STACS**, i.e. the methods "depth_profiles", "ratio_plotter" and everything they depend on, as a C standalone.
2. **Create a Python extension module** containing the functions that are significantly faster in C, following the example of NumPy.
3. **Include the NVIDIA CUDA framework** to enable parallel mass computation on GPUs.

## Support
For bug reports or feature requests, please contact maximilian.suhr@frm2.tum.de.

## Contributing
Contributions are greatly welcome.

## Authors and acknowledgment
Implementation of the original Python version:
- Leon Chryssos
- Lucian Mathes
- Vassily Burwitz

Translation to C:
- Maximilian Suhr

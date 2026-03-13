# CSTACS

## Description
CSTACS (read "C-STACS") is a playground for the re-implementation of the Python project [STACS](https://gitlab.lrz.de/tum-frm2-positrons/stacs), a library for evaluating (coincidence) Doppler-broadening spectroscopy data, with different tools, including GUIs.

The name CSTACS comes from the fact that this project was initially just meant as a C version of STACS to offer a better performance library to those who are familiar with C.

The core computational libraries, the GUIs and the interface layers are designed to be modular, allowing them to be combined arbitrarily. This way, the performance critical part can be done, for example, in Rust, while the graphics rendering can be done in JavaScript.

Since core library and GUI are not generally of the same language, an interface layer can brigde the gap. There are different ways to do that, and (if time allows) they will all be implemented in this project to benchmark them against each other:

- Making a server process that can use the core library directly and communicate with the GUI via a common IPC protocol, such as
  - HTTP
  - [gRPC](https://grpc.io/)
- Putting foreign language bindings on top of the libraries. For example, Python bindings on the C library allow a Python GUI to use the C library directly.

In the following and thoughout this project, the server process that makes the core library available to the GUI is referred to as "engine".

Another approach is to write monoliths, i.e., GUI and engine are of the same language, run in (possibly) just a single process and are closely integrated.

To use any of the components, refer to the READMEs in the respective folders.

## The Core Libraries

Currently, the core computational library exists in three other languages: C, C++, and Rust. The typical stages of development of such a library look like this:

1) Classes or structs to represent the data
2) A data importer for at least N42 and PNG files
3) Analysis of `SingleSpectra` (DBS), i.e. calculation of lineshape parameters
4) Analysis of `CoincidenceSpectra` (CDBS), i.e. slicing, binning, and ratio curve calculation.
5) Tools for, e.g., sorting, filtering, averaging, summing, and plotting of campaign data.

The current core libraries are at the following stages:

- C: 3
- C++: 2 (almost 3)
- Rust: 3

The Python original is at stage 5 (branch `coinc_dp`), or 4 (branch `main`).

More languages are not planned, but Julia might be added as an exercise or even for benchmarks.

## The GUIs

Currently, there is only one GUI: JavaScript (using [Angular](https://angular.dev/) and [Electron](https://www.electronjs.org/)) and it communicates with the engine via HTTP.
An HTTP server, currently, only exists for the Python library.

The implementation of GUIs with the following frameworks is planned:

- Qt (C++)
- PyQt (Python)

A C++ monolith would also be interesting to benchmark.

## Benchmarks

Coming soon...

## Disclaimers

Each part of this project is in the development stage and not guaranteed to work or produce accurate results. Use at your own peril.

Changelogs are nice, but currently not used in this project. They will be, if anything is mature enough to be published or if enough colleagues start using the GUIs offered by this project.

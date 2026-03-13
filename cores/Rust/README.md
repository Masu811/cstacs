# CSTACS - Rust Core

## Basic Usage

To include the CSTACS Rust Core in a Rust project of yours, add
```toml
[dependencies]
cstacs = { path = "/path/to/cstacs/cores/Rust/"}
```
to your `Cargo.toml`.

Assume we have a folder `measurement/` filled with one `measurement_1.n42` file and corresponding `.png` file. We can import the file's spectra into a `DopplerMeasurement` with `Single`- and `CoincidenceSpectra` in the following way.
```rust
use cstacs::importer::n42_importer::import_n42;

fn main() -> anyhow::Result<()> {
    let m = import_n42("measurements/measurement_1.n42")?;

    println!("{}", m.shape());
    
    Ok(())
}
```

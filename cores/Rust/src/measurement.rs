use std::collections::{HashMap, BTreeMap};
use std::fmt::{self, Display};

use crate::coinc::CoincidenceSpectrum;
use crate::importer::n42_importer::{ImportError, import_n42};
use crate::single::SingleSpectrum;

#[derive(Debug, Clone, Copy)]
pub struct DopplerMeasurementShape {
    pub s: usize,
    pub c: usize,
}

impl Display for DopplerMeasurementShape {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        write!(f, "Shape(s={}, c={})", self.s, self.c)
    }
}

pub struct DopplerMeasurement {
    filename: Option<String>,
    pub name: Option<String>,
    pub singles: BTreeMap<String, SingleSpectrum>,
    pub coinc: BTreeMap<String, CoincidenceSpectrum>,
    pub metadata: HashMap<String, String>,
}

impl DopplerMeasurement {
    pub fn new() -> Self {
        DopplerMeasurement {
            filename: None,
            name: None,
            singles: BTreeMap::new(),
            coinc: BTreeMap::new(),
            metadata: HashMap::new(),
        }
    }

    pub fn import(&mut self, filepath: &str) -> Result<(), ImportError> {
        let m = import_n42(filepath)?;

        self.filename = Some(filepath.into());
        self.name = self.filename.clone();
        self.singles = m.singles;
        self.coinc = m.coinc;
        self.metadata = m.metadata;

        Ok(())
    }

    pub fn shape(&self) -> DopplerMeasurementShape {
        DopplerMeasurementShape {
            s: self.singles.len(),
            c: self.coinc.len(),
        }
    }

    pub fn analyze_singles() {
        todo!();
    }

    pub fn analyze_coinc() {
        todo!();
    }
}

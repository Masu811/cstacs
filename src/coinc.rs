use crate::utils::Spectrum2D;

pub struct CoincidenceSpectrum {
    hist: Spectrum2D,
    detpair: String,
    ecal: ((f64, f64), (f64, f64)),
    // s: f64,
    // ds: f64,
    // w: f64,
    // dw: f64,
    // counts: u64,
    // dcounts: f64,
    // peak_counts: f64,
    // dpeak_counts: f64,
}

impl CoincidenceSpectrum {
    pub fn new(
        hist: Spectrum2D, detpair: String, ecal: ((f64, f64), (f64, f64))
    ) -> Self {
        CoincidenceSpectrum {
            hist: hist,
            detpair: detpair,
            ecal: ecal,
            // s: f64::NAN,
            // ds: f64::NAN,
            // w: f64::NAN,
            // dw: f64::NAN,
            // counts: 0,
            // dcounts: f64::NAN,
            // peak_counts: f64::NAN,
            // dpeak_counts: f64::NAN,
        }
    }

    pub fn detpair(&self) -> &str {
        self.detpair.as_str()
    }

    pub fn set_detair(&mut self, detpair: String) {
        self.detpair = detpair;
    }

    pub fn hist(&self) -> &Spectrum2D {
        &self.hist
    }

    pub fn set_hist(&mut self, hist: Spectrum2D) {
        self.hist = hist;
    }

    pub fn ecal(&self) -> &((f64, f64), (f64, f64)) {
        &self.ecal
    }

    pub fn set_ecal(&mut self, ecal: ((f64, f64), (f64, f64))) {
        self.ecal = ecal;
    }

    pub fn analyze() {
        todo!();
    }
}

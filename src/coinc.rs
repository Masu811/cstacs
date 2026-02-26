use crate::utils::Spectrum2D;

pub struct CoincidenceSpectrum {
    hist: Spectrum2D,
    detpair: String,
    ecal: ((f32, f32), (f32, f32)),
    // s: f32,
    // ds: f32,
    // w: f32,
    // dw: f32,
    // counts: u64,
    // dcounts: f32,
    // peak_counts: f32,
    // dpeak_counts: f32,
}

impl CoincidenceSpectrum {
    pub fn new(
        hist: Spectrum2D, detpair: String, ecal: ((f32, f32), (f32, f32))
    ) -> Self {
        CoincidenceSpectrum {
            hist: hist,
            detpair: detpair,
            ecal: ecal,
            // s: f32::NAN,
            // ds: f32::NAN,
            // w: f32::NAN,
            // dw: f32::NAN,
            // counts: 0,
            // dcounts: f32::NAN,
            // peak_counts: f32::NAN,
            // dpeak_counts: f32::NAN,
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

    pub fn ecal(&self) -> &((f32, f32), (f32, f32)) {
        &self.ecal
    }

    pub fn set_ecal(&mut self, ecal: ((f32, f32), (f32, f32))) {
        self.ecal = ecal;
    }

    pub fn analyze() {
        todo!();
    }
}

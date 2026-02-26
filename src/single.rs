use crate::utils::Spectrum;

pub struct SingleSpectrum {
    spectrum: Spectrum,
    detname: String,
    ecal: (f32, f32),
    // s: f32,
    // ds: f32,
    // w: f32,
    // dw: f32,
    // v2p: f32,
    // dv2p: f32,
    // counts: u64,
    // dcounts: f32,
    // peak_counts: f32,
    // dpeak_counts: f32,
}

impl SingleSpectrum {
    pub fn new(spectrum: Spectrum, detname: String, ecal: (f32, f32)) -> Self {
        SingleSpectrum {
            spectrum: spectrum,
            detname: detname,
            ecal: ecal,
            // s: f32::NAN,
            // ds: f32::NAN,
            // w: f32::NAN,
            // dw: f32::NAN,
            // v2p: f32::NAN,
            // dv2p: f32::NAN,
            // counts: 0,
            // dcounts: f32::NAN,
            // peak_counts: f32::NAN,
            // dpeak_counts: f32::NAN,
        }
    }

    pub fn detname(&self) -> &str {
        self.detname.as_str()
    }

    pub fn set_detname(&mut self, detname: String) {
        self.detname = detname;
    }

    pub fn spectrum(&self) -> &Spectrum {
        &self.spectrum
    }

    pub fn set_spectrum(&mut self, spectrum: Spectrum) {
        self.spectrum = spectrum;
    }

    pub fn ecal(&self) -> &(f32, f32) {
        &self.ecal
    }

    pub fn set_ecal(&mut self, ecal: (f32, f32)) {
        self.ecal = ecal;
    }

    pub fn analyze(&mut self) {
        todo!();
    }

    pub fn calibrate() {
        todo!();
    }

    pub fn calculate_eres() {
        todo!();
    }
}

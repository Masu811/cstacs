use thiserror::Error;
use statrs::statistics::Statistics;

use crate::fitting::{FitError, fit_gaussian, fit_comb, mod_erf};
use crate::utils::{Spectrum, spectrum_match};
use crate::M_POSITRON_KEV;

#[derive(Debug, Error)]
pub enum AnalysisError {
    #[error("Provided peak width results in a peak of less than 3 channels")]
    PeakWidthError,

    #[error("No peak found. Maybe your energy calibration is faulty?")]
    NoPeakFound,

    #[error(
        "Provided {param} violated the following constraints: {constraints}"
    )]
    InputError {
        param: String,
        constraints: String,
    },

    #[error("Error during fitting")]
    FitError {
        #[from]
        inner: FitError,
    },
}

#[derive(Debug, PartialEq)]
pub enum FollowPeakOrder {
    Zeroth,
    First,
    None,
}

pub struct SingleSpectrum {
    spectrum: Spectrum,
    detname: String,
    ecal: (f64, f64),
    s: f64,
    ds: f64,
    w: f64,
    dw: f64,
    v2p: f64,
    dv2p: f64,
    counts: u64,
    dcounts: f64,
    peak_counts: f64,
    dpeak_counts: f64,
}

impl SingleSpectrum {
    pub fn new(spectrum: Spectrum, detname: String, ecal: (f64, f64)) -> Self {
        SingleSpectrum {
            spectrum: spectrum,
            detname: detname,
            ecal: ecal,
            s: f64::NAN,
            ds: f64::NAN,
            w: f64::NAN,
            dw: f64::NAN,
            v2p: f64::NAN,
            dv2p: f64::NAN,
            counts: 0,
            dcounts: f64::NAN,
            peak_counts: f64::NAN,
            dpeak_counts: f64::NAN,
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

    pub fn ecal(&self) -> &(f64, f64) {
        &self.ecal
    }

    pub fn set_ecal(&mut self, ecal: (f64, f64)) {
        self.ecal = ecal;
    }

    pub fn s(&self) -> &f64 {
        &self.s
    }

    pub fn ds(&self) -> &f64 {
        &self.ds
    }

    pub fn w(&self) -> &f64 {
        &self.w
    }

    pub fn dw(&self) -> &f64 {
        &self.dw
    }

    pub fn v2p(&self) -> &f64 {
        &self.v2p
    }

    pub fn dv2p(&self) -> &f64 {
        &self.dv2p
    }

    fn get_peak_bnd_idx(
        &self, peak_width: f64, right_side: bool,
    ) -> usize {
        let direction: f64 = match right_side {
            true => 1.,
            false => -1.,
        };

        let h = direction * peak_width / 2.;
        let peak_bnd_idx = ((M_POSITRON_KEV + h) - self.ecal.0) / self.ecal.1;

        if peak_bnd_idx < 0. {
            0
        } else if peak_bnd_idx > (self.spectrum.len() - 1) as f64 {
            self.spectrum.len() - 1
        } else {
            peak_bnd_idx as usize
        }
    }

    fn extract_peak(
        &self, peak_width: f64
    ) -> Result<(Vec<f64>, Vec<f64>), AnalysisError> {
        if peak_width <= 0. {
            return Err(AnalysisError::PeakWidthError);
        }

        let left_idx = self.get_peak_bnd_idx(peak_width, false);
        let right_idx = self.get_peak_bnd_idx(peak_width, true);

        let n_ch_in_peak = right_idx - left_idx + 1;

        if n_ch_in_peak < 3 {
            return Err(AnalysisError::PeakWidthError);
        }

        let peak = spectrum_match!(&self.spectrum, spectrum => {
            spectrum[left_idx..=right_idx].iter().map(|x| *x as f64).collect()
        });

        let energies: Vec<f64> = (left_idx..=right_idx).map(
            |i| self.ecal.0 + self.ecal.1 * i as f64
        ).collect();

        Ok((energies, peak))
    }

    fn check_peak_std(
        &self, peak: &[f64],
    ) -> Result<(), AnalysisError> {
        let variance = peak.variance();

        if variance < 1. {
            Err(AnalysisError::NoPeakFound)
        } else {
            Ok(())
        }
    }

    fn get_peak_position(
        &self, peak: &[f64], energies: &[f64]
    ) -> Result<f64, AnalysisError> {
        Ok(fit_gaussian(energies, peak)?[1])
    }

    fn correct_ecal(
        &mut self,
        follow_peak_order: FollowPeakOrder,
        peak: &[f64],
        energies: &[f64],
    ) -> Result<(), AnalysisError> {
        let current_center = self.get_peak_position(peak, energies)?;

        match follow_peak_order {
            FollowPeakOrder::Zeroth => {
                let new_coeff = self.ecal.0 - current_center + M_POSITRON_KEV;
                self.set_ecal((new_coeff, self.ecal.1));
            },
            FollowPeakOrder::First => {
                let new_coeff = self.ecal.1
                    * (M_POSITRON_KEV - self.ecal.0)
                    / (current_center - self.ecal.0);
                self.set_ecal((self.ecal.0, new_coeff));
            },
            FollowPeakOrder::None => {}
        };

        Ok(())
    }

    fn correct_peak(
        &self, peak: &mut [f64], energies: &[f64],
    ) -> Result<(), AnalysisError> {
        let opt = fit_comb(energies, peak)?;

        let corr = mod_erf(energies, opt[1], opt[2], opt[3], opt[4]);

        for (y, c) in peak.iter_mut().zip(corr) {
            *y = *y - c;
        }

        Ok(())
    }

    fn integrate_peak(
        &self,
        peak: &[f64],
        energies: &[f64],
        left_e: f64,
        right_e: f64,
    ) -> Result<f64, AnalysisError> {
        let mut counts: f64 = 0.;

        let Some(e0) = energies.first() else {
            return Err(AnalysisError::PeakWidthError);
        };

        let Some(e1) = energies.last() else {
            return Err(AnalysisError::PeakWidthError);
        };

        let ecal = (e0, (e1 - e0) / energies.len() as f64);

        let left_inner_idx = ((left_e - ecal.0) / ecal.1) as usize + 1;
        let right_inner_idx = ((right_e - ecal.0) / ecal.1) as usize - 1;

        counts += peak[left_inner_idx..=right_inner_idx].iter().sum::<f64>();

        let left_frac = ((left_e - ecal.0) % ecal.1) / ecal.1;
        let right_frac = ((right_e - ecal.0) % ecal.1) / ecal.1;

        counts += peak[left_inner_idx-1] * left_frac;
        counts += peak[right_inner_idx+1] * right_frac;

        Ok(counts)
    }

    fn integrate_spectrum(
        &self,
        left_e: f64,
        right_e: f64,
    ) -> Result<f64, AnalysisError> {
        spectrum_match!(&self.spectrum, spectrum => {
            let mut counts: f64 = 0.;
            let ecal = self.ecal;
            let left_inner_idx = ((left_e - ecal.0) / ecal.1) as usize + 1;
            let right_inner_idx = ((right_e - ecal.0) / ecal.1) as usize - 1;

            counts += spectrum[left_inner_idx..=right_inner_idx]
                .iter()
                .map(|&x| x as f64)
                .sum::<f64>();

            let left_frac = ((left_e - ecal.0) % ecal.1) / ecal.1;
            let right_frac = ((right_e - ecal.0) % ecal.1) / ecal.1;

            counts += spectrum[left_inner_idx-1] as f64 * left_frac;
            counts += spectrum[right_inner_idx+1] as f64 * right_frac;

            Ok(counts)
        })
    }

    fn calc_lineshape_params(
        &mut self,
        peak: &[f64],
        energies: &[f64],
        s_width: f64,
        w_width: f64,
        w_dist: f64,
        w_rightonly: bool,
        v2p_bounds: (f64, f64, f64, f64),
    ) -> Result<(), AnalysisError> {
        let mut counts: u64 = 0;

        spectrum_match!(&self.spectrum, spectrum => {
            for elem in spectrum {
                counts += *elem as u64;
            }
        });

        self.counts = counts;
        self.dcounts = (counts as f64).sqrt();

        let peak_counts = peak.iter().sum();

        self.peak_counts = peak_counts;
        self.dpeak_counts = peak_counts.sqrt();

        let s_area = self.integrate_peak(
            &peak,
            &energies,
            M_POSITRON_KEV - s_width,
            M_POSITRON_KEV + s_width,
        )?;

        let w_r_area = self.integrate_peak(
            &peak,
            &energies,
            M_POSITRON_KEV + w_dist,
            M_POSITRON_KEV + w_dist + w_width,
        )?;

        let w_l_area = if w_rightonly == true {
            0.
        } else {
            self.integrate_peak(
                &peak,
                &energies,
                M_POSITRON_KEV - w_dist - w_width,
                M_POSITRON_KEV - w_dist,
            )?
        };

        let w_area = w_l_area + w_r_area;

        let valley_area = self.integrate_spectrum(v2p_bounds.0, v2p_bounds.1)?;
        let v_peak_area = self.integrate_spectrum(v2p_bounds.2, v2p_bounds.3)?;

        let s = s_area / peak_counts;
        let w = w_area / peak_counts;
        let v2p = valley_area / v_peak_area;

        self.s = s;
        self.ds = (s * (1. - s) / peak_counts).sqrt();

        self.w = w;
        self.dw = (w * (1. - w) / peak_counts).sqrt();

        self.v2p = v2p;
        self.dv2p = v2p * (1. / valley_area + 1. / v_peak_area).sqrt();

        Ok(())
    }

    fn check_analyze_params(
        &self,
        s_width: f64,
        w_width: f64,
        w_dist: f64,
        peak_width: f64,
        v2p_bounds: (f64, f64, f64, f64),
    ) -> Result<(), AnalysisError> {
        if s_width <= 0. {
            return Err(AnalysisError::InputError {
                param: "s_width".into(),
                constraints: "s_width > 0".into(),
            });
        }

        if w_width <= 0. {
            return Err(AnalysisError::InputError {
                param: "w_width".into(),
                constraints: "w_width > 0".into(),
            });
        }

        if w_dist <= 0. {
            return Err(AnalysisError::InputError {
                param: "w_dist".into(),
                constraints: "w_dist > 0".into(),
            });
        }

        if peak_width <= 0. {
            return Err(AnalysisError::InputError {
                param: "peak_width".into(),
                constraints: "peak_width > 0".into(),
            });
        }

        let (a, b, c, d) = v2p_bounds;

        if ![a, b, c, d].iter().all(|&x| x > 0.) {
            return Err(AnalysisError::InputError {
                param: "v2p_bounds".into(),
                constraints: "all v2p_bounds > 0".into(),
            });
        }

        if b <= a {
            return Err(AnalysisError::InputError {
                param: "v2p_bounds".into(),
                constraints: "v2p_bounds.1 > v2p_bounds.0".into(),
            });
        }

        if d <= c {
            return Err(AnalysisError::InputError {
                param: "v2p_bounds".into(),
                constraints: "v2p_bounds.3 > v2p_bounds.2".into(),
            });
        }

        Ok(())
    }

    pub fn analyze(
        &mut self,
        s_width: f64,
        w_width: f64,
        w_dist: f64,
        w_rightonly: bool,
        peak_width: f64,
        bg_corr: bool,
        v2p_bounds: (f64, f64, f64, f64),
        follow_peak_order: FollowPeakOrder,
    ) -> Result<(), AnalysisError> {
        self.check_analyze_params(
            s_width, w_width, w_dist, peak_width, v2p_bounds
        )?;

        let (mut energies, mut peak) = self.extract_peak(peak_width)?;

        self.check_peak_std(&peak)?;

        if follow_peak_order != FollowPeakOrder::None {
            self.correct_ecal(follow_peak_order, &peak, &energies)?;
            (energies, peak) = self.extract_peak(peak_width)?;
        }

        if bg_corr == true {
            self.correct_peak(&mut peak, &energies)?;
        }

        self.calc_lineshape_params(
            &peak, &energies, s_width, w_width, w_dist, w_rightonly, v2p_bounds,
        )?;

        Ok(())
    }

    pub fn calibrate() {
        todo!();
    }

    pub fn calculate_eres() {
        todo!();
    }
}

pub enum Spectrum {
    U8(Vec<u8>),
    U16(Vec<u16>),
    U32(Vec<u32>),
    U64(Vec<u64>),
}

macro_rules! spectrum_match {
    ($s:expr, $v:ident => $body:expr) => {
        match $s {
            Spectrum::U8($v) => $body,
            Spectrum::U16($v) => $body,
            Spectrum::U32($v) => $body,
            Spectrum::U64($v) => $body,
        }
    };
}

pub(crate) use spectrum_match;

impl From<Vec<u8>> for Spectrum {
    fn from(other: Vec<u8>) -> Spectrum {
        Spectrum::U8(other)
    }
}

impl From<Vec<u16>> for Spectrum {
    fn from(other: Vec<u16>) -> Spectrum {
        let max = other.iter().max().unwrap_or(&u16::MAX);

        if max <= &u8::MAX.into() {
            Spectrum::U8(other.iter().map(|&x| x as u8).collect())
        } else {
            Spectrum::U16(other)
        }
    }
}

impl From<Vec<u32>> for Spectrum {
    fn from(other: Vec<u32>) -> Spectrum {
        let max = other.iter().max().unwrap_or(&u32::MAX);

        if max <= &u8::MAX.into() {
            Spectrum::U8(other.iter().map(|&x| x as u8).collect())
        } else if max <= &u16::MAX.into() {
            Spectrum::U16(other.iter().map(|&x| x as u16).collect())
        } else {
            Spectrum::U32(other)
        }
    }
}

impl From<Vec<u64>> for Spectrum {
    fn from(other: Vec<u64>) -> Spectrum {
        let max = other.iter().max().unwrap_or(&u64::MAX);

        if max <= &u8::MAX.into() {
            Spectrum::U8(other.iter().map(|&x| x as u8).collect())
        } else if max <= &u16::MAX.into() {
            Spectrum::U16(other.iter().map(|&x| x as u16).collect())
        } else if max <= &u16::MAX.into() {
            Spectrum::U32(other.iter().map(|&x| x as u32).collect())
        } else {
            Spectrum::U64(other)
        }
    }
}

impl Spectrum {
    pub fn len(&self) -> usize {
        spectrum_match!(self, spectrum => spectrum.len())
    }

    pub fn is_empty(&self) -> bool {
        self.len() == 0
    }
}

pub enum SpectrumSlice<'a> {
    U8(&'a [u8]),
    U16(&'a [u16]),
    U32(&'a [u32]),
    U64(&'a [u64]),
}

macro_rules! spectrum_slice_match {
    ($s:expr, $v:ident => $body:expr) => {
        match $s {
            SpectrumSlice::U8($v) => $body,
            SpectrumSlice::U16($v) => $body,
            SpectrumSlice::U32($v) => $body,
            SpectrumSlice::U64($v) => $body,
        }
    };
}

pub(crate) use spectrum_slice_match;

impl<'a> From<&'a [u8]> for SpectrumSlice<'a> {
    fn from(other: &'a [u8]) -> SpectrumSlice<'a> {
        SpectrumSlice::U8(other)
    }
}

impl<'a> From<&'a [u16]> for SpectrumSlice<'a> {
    fn from(other: &'a [u16]) -> SpectrumSlice<'a> {
        SpectrumSlice::U16(other)
    }
}

impl<'a> From<&'a [u32]> for SpectrumSlice<'a> {
    fn from(other: &'a [u32]) -> SpectrumSlice<'a> {
        SpectrumSlice::U32(other)
    }
}

impl<'a> From<&'a [u64]> for SpectrumSlice<'a> {
    fn from(other: &'a [u64]) -> SpectrumSlice<'a> {
        SpectrumSlice::U64(other)
    }
}

impl<'a> SpectrumSlice<'a> {
    pub fn len(self) -> usize {
        spectrum_slice_match!(self, spectrum => spectrum.len())
    }

    pub fn variance(self) -> Option<f64> {
        spectrum_slice_match!(self, spectrum => {
            let mut count: u64 = 0;
            let mut mean: f64 = 0.;
            let mut m2: f64 = 0.;

            for value in spectrum {
                count += 1;
                let x = *value as f64;
                let delta = x - mean;
                mean += delta / count as f64;
                let delta2 = x - mean;
                m2 += delta * delta2;
            }

            if count > 1 {
                Some(m2 / (count as f64 - 1.))
            } else {
                None
            }
        })
    }
}

pub enum Spectrum2D {
    U8(Vec<u8>),
    U16(Vec<u16>),
    U32(Vec<u32>),
    U64(Vec<u64>),
}

impl From<Vec<u8>> for Spectrum2D {
    fn from(other: Vec<u8>) -> Spectrum2D {
        let spectrum = Spectrum::from(other);

        match spectrum {
            Spectrum::U8(spectrum) => Spectrum2D::U8(spectrum),
            Spectrum::U16(spectrum) => Spectrum2D::U16(spectrum),
            Spectrum::U32(spectrum) => Spectrum2D::U32(spectrum),
            Spectrum::U64(spectrum) => Spectrum2D::U64(spectrum),
        }
    }
}

impl From<Vec<u16>> for Spectrum2D {
    fn from(other: Vec<u16>) -> Spectrum2D {
        let spectrum = Spectrum::from(other);

        match spectrum {
            Spectrum::U8(spectrum) => Spectrum2D::U8(spectrum),
            Spectrum::U16(spectrum) => Spectrum2D::U16(spectrum),
            Spectrum::U32(spectrum) => Spectrum2D::U32(spectrum),
            Spectrum::U64(spectrum) => Spectrum2D::U64(spectrum),
        }
    }
}

impl From<Vec<u32>> for Spectrum2D {
    fn from(other: Vec<u32>) -> Spectrum2D {
        let spectrum = Spectrum::from(other);

        match spectrum {
            Spectrum::U8(spectrum) => Spectrum2D::U8(spectrum),
            Spectrum::U16(spectrum) => Spectrum2D::U16(spectrum),
            Spectrum::U32(spectrum) => Spectrum2D::U32(spectrum),
            Spectrum::U64(spectrum) => Spectrum2D::U64(spectrum),
        }
    }
}

impl From<Vec<u64>> for Spectrum2D {
    fn from(other: Vec<u64>) -> Spectrum2D {
        let spectrum = Spectrum::from(other);

        match spectrum {
            Spectrum::U8(spectrum) => Spectrum2D::U8(spectrum),
            Spectrum::U16(spectrum) => Spectrum2D::U16(spectrum),
            Spectrum::U32(spectrum) => Spectrum2D::U32(spectrum),
            Spectrum::U64(spectrum) => Spectrum2D::U64(spectrum),
        }
    }
}

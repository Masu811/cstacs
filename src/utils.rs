pub enum Spectrum {
    U8(Vec<u8>),
    U16(Vec<u16>),
    U32(Vec<u32>),
    U64(Vec<u64>),
}

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

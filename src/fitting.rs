use nalgebra::DVector;
use statrs::function::erf::erf;
use thiserror::Error;
use varpro::prelude::*;
use varpro::problem::*;
use varpro::solvers::levmar::LevMarSolver;

const TWO_OVER_SQRT_PI: f64 = 1.1283791670955126;

#[derive(Debug, Error)]
pub enum FitError {
    #[error("Data contains only NaN values")]
    AllNANValues,

    #[error("ModelBuilderError")]
    ModelBuilderError {
        #[from]
        inner: varpro::model::builder::error::ModelBuildError,
    },

    #[error("SeparableProblemBuilderError")]
    SeparableProblemBuilderError {
        #[from]
        inner: varpro::problem::SeparableProblemBuilderError,
    },

    #[error("Something went wrong during least squares fit")]
    RuntimeError,
}

fn gauss_basis(
    x: &DVector<f64>, x0: f64, sig: f64
) -> DVector<f64> {
    x.map(|x| {
        let u = (x - x0) / sig;
        (-0.5 * u*u).exp()
    })
}

fn gaussian_dx0(
    x: &DVector<f64>, x0: f64, sig: f64
) -> DVector<f64> {
    x.map(|x| {
        let u = (x - x0) / sig;
        (-0.5 * u*u).exp() * u / sig
    })
}

fn gaussian_dsig(
    x: &DVector<f64>, x0: f64, sig: f64
) -> DVector<f64> {
    x.map(|x| {
        let u = (x - x0) / sig;
        (-0.5 * u*u).exp() * u * u / sig
    })
}

pub fn fit_gaussian(
    x: &[f64], y: &[f64],
) -> Result<[f64; 3], FitError> {
    let model = SeparableModelBuilder::<f64>::new(&["x0", "sig"])
        .function(&["x0", "sig"], gauss_basis)
        .partial_deriv("x0", gaussian_dx0)
        .partial_deriv("sig", gaussian_dsig)
        .independent_variable(DVector::from_column_slice(x))
        .initial_parameters(vec![511., 1.])
        .build()?;

    let problem = SeparableProblemBuilder::new(model)
        .observations(DVector::from_column_slice(y))
        .build()?;

    let Ok(fit_result) = LevMarSolver::default().solve(problem) else {
        return Err(FitError::RuntimeError);
    };

    let alpha = fit_result.nonlinear_parameters();

    let Some(c) = fit_result.linear_coefficients() else {
        return Err(FitError::RuntimeError);
    };

    Ok([c[0], alpha[0], alpha[1]])
}

fn erf_basis(
    x: &DVector<f64>, x0: f64, sig: f64,
) -> DVector<f64> {
    x.map(|x| {
        let u = (x - x0) / sig;
        erf(u)
    })
}

fn erf_dx0(
    x: &DVector<f64>, x0: f64, sig: f64,
) -> DVector<f64> {
    x.map(|x| {
        let u = (x - x0) / sig;
        let e = (-u * u).exp();
        -TWO_OVER_SQRT_PI * e / sig
    })
}

fn erf_dsig(
    x: &DVector<f64>, x0: f64, sig: f64,
) -> DVector<f64> {
    x.map(|x| {
        let u = (x - x0) / sig;
        let e = (-u * u).exp();
        TWO_OVER_SQRT_PI * e * u / sig
    })
}

pub fn fit_comb(
    x: &[f64], y: &[f64],
) -> Result<[f64; 5], FitError> {
    let model = SeparableModelBuilder::<f64>::new(&["x0", "sig"])
        .function(&["x0", "sig"], gauss_basis)
        .partial_deriv("x0", gaussian_dx0)
        .partial_deriv("sig", gaussian_dsig)
        .function(&["x0", "sig"], erf_basis)
        .partial_deriv("x0", erf_dx0)
        .partial_deriv("sig", erf_dsig)
        .invariant_function(|x| DVector::from_element(x.len(), 1.))
        .independent_variable(DVector::from_column_slice(x))
        .initial_parameters(vec![511., 1.])
        .build()?;

    let problem = SeparableProblemBuilder::new(model)
        .observations(DVector::from_column_slice(y))
        .build()?;

    let Ok(fit_result) = LevMarSolver::default().solve(problem) else {
        return Err(FitError::RuntimeError);
    };

    let alpha = fit_result.nonlinear_parameters();

    let Some(c) = fit_result.linear_coefficients() else {
        return Err(FitError::RuntimeError);
    };

    Ok([c[0], c[1], alpha[0], alpha[1], c[2]])
}

pub fn mod_erf(
    x: &[f64], amp: f64, x0: f64, sig: f64, off: f64
) -> Vec<f64> {
    x.iter().map(|x| {
        let u = (x - x0) / sig;
        amp * erf(u) + off
    }).collect()
}

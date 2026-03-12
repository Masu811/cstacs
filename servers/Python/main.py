from pathlib import Path
from uuid import uuid4 as uuid

import numpy as np
from fastapi import FastAPI
from fastapi.responses import JSONResponse

from stacs.coinc import CoincidenceSpectrum
from stacs.importer import SingleSpectrum
from stacs import MultiCampaign, MeasurementCampaign, DopplerMeasurement

data = dict()

app = FastAPI()

@app.get("/health")
def health():
    return JSONResponse({"status": "ok"})

def tree(m: MultiCampaign | MeasurementCampaign | DopplerMeasurement):
    if isinstance(m, DopplerMeasurement):
        return {
            "id": str(uuid()),
            "name": m.name,
            "singles": [s.detname for s in m.singles],
            "coinc": [c.detpair for c in m.coinc],
        }

    if isinstance(m, MeasurementCampaign):
        return {
            "id": str(uuid()),
            "name": m.name,
            "measurements": [tree(n) for n in m],
        }

    return {
        "id": str(uuid()),
        "name": m.name,
        "campaigns": [tree(n) for n in m]
    }

@app.get("/import_data")
def import_data(path: str):
    mult = MultiCampaign(
        path,
        autocompute_singles=False,
        autocompute_coinc=False,
    )
    t = tree(mult)
    data[t["id"]] = mult
    return JSONResponse(t)

@app.get("/delete_data")
def delete_data(id: str):
    data.pop(id)

def scalarize(x):
    return str(np.array(x).tolist())

def mult_to_json(x: MultiCampaign):
    vals = [
        "name", "path", "directory"
    ]
    return {"__name__": "MultiCampaign"} | {
        val: scalarize(getattr(x, val)) for val in vals
    } | {"length": len(x)}

def mc_to_json(x: MeasurementCampaign):
    vals = [
        "name", "path", "directory"
    ]
    return {"__name__": "MeasurementCampaign"} | {
        val: scalarize(getattr(x, val)) for val in vals
    } | {"length": len(x)}

def m_to_json(x: DopplerMeasurement):
    vals = [
        "name", "path", "directory", "filename", "filetype"
    ]
    return {"__name__": "DopplerMeasurement"} | {
        val: scalarize(getattr(x, val)) for val in vals
    } | {key: str(val) for key, val in x.metadata.items()}

def s_to_json(x: SingleSpectrum):
    vals = [
        "detname", "ecal", "eres", "peak_center", "peak_counts", "dpeak_counts",
        "counts", "dcounts", "s", "ds", "w", "dw", "v2p", "dv2p",
    ]
    return {"__name__": "SingleSpectrum"} | {
        val: scalarize(getattr(x, val)) for val in vals
    }

def c_to_json(x: CoincidenceSpectrum):
    vals = [
        "detpair", "ecal", "eres", "peak_counts", "dpeak_counts",
        "counts", "dcounts", "s", "ds", "w", "dw", "roi_params"
    ]
    return {"__name__": "CoincidenceSpectrum"} | {
        val: scalarize(getattr(x, val)) for val in vals
    }

def to_json(
    x: MultiCampaign
    | MeasurementCampaign
    | DopplerMeasurement
    | SingleSpectrum
    | CoincidenceSpectrum
):
    if isinstance(x, MultiCampaign):
        return mult_to_json(x)
    if isinstance(x, MeasurementCampaign):
        return mc_to_json(x)
    if isinstance(x, DopplerMeasurement):
        return m_to_json(x)
    if isinstance(x, SingleSpectrum):
        return s_to_json(x)
    return c_to_json(x)

@app.get("/getMetadata/{path:path}")
def get_metadata(path: str):
    idcs = path.split("/")

    types = [str, int, int, str]

    x = data
    for i, type in zip(idcs, types):
        x = x[type(i)]

    return JSONResponse(to_json(x))

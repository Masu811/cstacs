from pathlib import Path
from enum import Enum

import numpy as np
from fastapi import FastAPI, WebSocket, WebSocketDisconnect
from fastapi.responses import JSONResponse
from pydantic.dataclasses import dataclass

from stacs.coinc import CoincidenceSpectrum
from stacs.single import SingleSpectrum
from stacs import MultiCampaign, MeasurementCampaign, DopplerMeasurement

data: list[MultiCampaign] = []

app = FastAPI()

@app.websocket("/health")
async def health(websocket: WebSocket):
    await websocket.accept()
    try:
        while True:
            await websocket.receive()
    except (WebSocketDisconnect, RuntimeError):
        print("Client disconnected")
        pass

def tree(m: MultiCampaign | MeasurementCampaign | DopplerMeasurement):
    if isinstance(m, DopplerMeasurement):
        return {
            "name": m.name,
            "singles": [s.detname for s in m.singles],
            "coinc": [c.detpair for c in m.coinc],
        }

    if isinstance(m, MeasurementCampaign):
        return {
            "name": m.name,
            "measurements": [tree(n) for n in m],
        }

    return {
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
    data.append(mult)
    return JSONResponse([tree(t) for t in data])

class Dtype(Enum):
    MULT = "MULT"
    MC = "MC"
    M = "M"
    S = "S"
    C = "C"

def selection_to_idcs(selection):
    fields = selection.split(",")

    types = [int, int, int, str]

    out = []

    for field, type in zip(fields, types):
        out.append(type(field))

    return out

@app.post("/delete_data")
def delete_data(selection: dict[Dtype, list[str]]):
    remove_mult = sorted(
        [selection_to_idcs(x) for x in selection[Dtype.MULT]],
        key=lambda x: x[0],
        reverse=True,
    )

    remove_mc = sorted(
        filter(
            lambda x: (
                not any (x[0] == y[0] for y in remove_mult)
            ),
           [selection_to_idcs(x) for x in selection[Dtype.MC]]
        ),
        key=lambda x: x[1],
        reverse=True,
    )

    remove_m = sorted(
        filter(
            lambda x: (
                not any (x[0] == y[0] for y in remove_mult)
                and not any(x[0] == y[0] and x[1] == y[1] for y in remove_mc)
            ),
           [selection_to_idcs(x) for x in selection[Dtype.M]]
        ),
        key=lambda x: x[2],
        reverse=True,
    )

    remove_s = sorted(
        filter(
            lambda x: (
                not any (x[0] == y[0] for y in remove_mult)
                and not any(x[0] == y[0] and x[1] == y[1] for y in remove_mc)
                and not any(x[0] == y[0] and x[1] == y[1] and x[2] == y[2] for y in remove_m)
            ),
           [selection_to_idcs(x) for x in selection[Dtype.S]]
        )
    )

    remove_c = sorted(
        filter(
            lambda x: (
                not any (x[0] == y[0] for y in remove_mult)
                and not any(x[0] == y[0] and x[1] == y[1] for y in remove_mc)
                and not any(x[0] == y[0] and x[1] == y[1] and x[2] == y[2] for y in remove_m)
            ),
           [selection_to_idcs(x) for x in selection[Dtype.C]]
        )
    )

    print(len(remove_mult), len(remove_mc), len(remove_m), len(remove_s))

    for idcs in remove_s:
        m = data[idcs[0]][idcs[1]][idcs[2]]
        s = m[idcs[3]]
        m.singles.remove(s)

    for idcs in remove_c:
        m = data[idcs[0]][idcs[1]][idcs[2]]
        c = m[idcs[3]]
        m.coinc.remove(c)

    for idcs in remove_m:
        data[idcs[0]][idcs[1]].measurements.pop(idcs[2])

    for idcs in remove_mc:
        data[idcs[0]].campaigns.pop(idcs[1])

    for idcs in remove_mult:
        data.pop(idcs[0])

    return JSONResponse([tree(t) for t in data])


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

@app.get("/getMetadata/{idcs}")
def get_metadata(idcs: str):
    keys = idcs.split("-")

    types = [int, int, int, str]

    x = data
    for i, type in zip(keys, types):
        x = x[type(i)]

    return JSONResponse(to_json(x))

from pathlib import Path
from uuid import uuid4 as uuid

from fastapi import FastAPI
from fastapi.responses import JSONResponse

import stacs
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
    mult = stacs.MultiCampaign(
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

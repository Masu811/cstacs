from fastapi import FastAPI

import stacs

app = FastAPI()

@app.get("/import")
def import_data(file_or_folder: str):
    return "This is message will self destruct in 10..."

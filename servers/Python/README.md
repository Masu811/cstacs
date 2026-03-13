# CSTACS - Python Server

## Requirements

The requirements are listed in the `requirements.txt`, except for the [STACS](https://gitlab.lrz.de/tum-frm2-positrons/stacs) Python library itself. Be sure to include it in your PYTHONPATH and have switched to branch `coinc_dp` (meaning you have to install it via git and not pip).

## Description

This server runs a [FastAPI](https://fastapi.tiangolo.com/) HTTP server to serve a GUI. At the present time, the GUI does not automatically start the server, but the user has to start it manually. You can restart the GUI without restarting the engine, but not the other way around.

## Basic Usage

Before or after having opened a GUI, start the Python engine via
```bash
uvicorn main:app --port 8000
```
Be sure that you have your venv activated, if necessary.

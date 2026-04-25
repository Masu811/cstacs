from dataclasses import dataclass
from datetime import datetime
import math
from typing import Callable, TypedDict

import numpy as np
from fastapi import HTTPException


@dataclass
class Parser:
    name: str
    args: dict[str, str]
    repr: str


class ParserDict(TypedDict):
    name: str
    args: dict[str, str]
    repr: str


def parse_parser(parser_dict: ParserDict) -> Callable:
    parser = Parser(**parser_dict)
    match parser.name:
        case "":
            return lambda x: x
        case "float":
            return float
        case "string":
            return str
        case "datetime":
            format = parser.args["format"]
            if format != "":
                return lambda x: datetime.strptime(x, format)
            else:
                return datetime.fromisoformat
        ########################################################################
        case "bool":
            return bool
        case "u8":
            return np.uint8
        case "u16":
            return np.uint16
        case "u32":
            return np.uint32
        case "u64":
            return np.uint64
        case "i8":
            return np.int8
        case "i16":
            return np.int16
        case "i32":
            return np.int32
        case "i64":
            return np.int64
        case "f32":
            return np.float32
        case "f64":
            return np.float64
        case "usize":
            return np.uintp
        case "to_scientific":
            return lambda x: f"{x:.2e}"
        case "to_hex":
            return hex
        case "from_hex":
            return lambda x: int(x, base=16)
        case "to_bin":
            return bin
        case "from_bin":
            return lambda x: int(x, 2)
        case "isfinite":
            return np.isfinite
        case "isnan":
            return np.isnan
        case "isinf":
            return np.isinf
        ########################################################################
        case "add":
            number = float(parser.args["number"])
            return lambda x: x + number
        case "sub":
            number = float(parser.args["number"])
            lambda x: x - number
        case "mul":
            number = float(parser.args["number"])
            lambda x: x * number
        case "div":
            number = float(parser.args["number"])
            lambda x: x / number
        case "round":
            number = int(parser.args["decimals"])
            return lambda x: round(x, number)
        case "floor":
            return math.floor
        case "ceil":
            return math.ceil
        case "trunc":
            return math.trunc
        case "fract":
            return lambda x: math.modf(x)[0]
        case "mod":
            number = float(parser.args["number"])
            return lambda x: math.fmod(x, number)
        case "floor_div":
            number = float(parser.args["number"])
            return lambda x: x // number
        case "square":
            return lambda x: x**2
        case "sqrt":
            return math.sqrt
        case "pow":
            number = float(parser.args["number"])
            return lambda x: pow(x, number)
        case "exp":
            base = float(parser.args["base"] or math.e)
            return lambda x: pow(base, x)
        case "log":
            base = float(parser.args["base"] or math.e)
            return lambda x: math.log(x, base)
        ########################################################################
        case "sin":
            return math.sin
        case "tan":
            return math.tan
        case "arcsin":
            return math.asin
        case "arctan":
            return math.atan
        case "deg":
            return math.degrees
        case "rad":
            return math.radians
        case "sinh":
            return math.sinh
        case "tanh":
            return math.tanh
        case "arsinh":
            return math.asinh
        case "artanh":
            return math.atanh
        case "erf":
            return math.erf
        case "gamma":
            return math.gamma
        ########################################################################
        case "lower":
            return lambda x: x.lower()
        case "upper":
            return lambda x: x.upper()
        case "capitalize":
            return lambda x: x.capitalize()
        case "title":
            return lambda x: x.title()
        case "replace":
            old = parser.args["old"]
            new = parser.args["new"]
            count = int(parser.args["count"])
            return lambda x: x.replace(old, new, count)
        case "slice":
            start = int(parser.args["start"])
            stop = int(parser.args["stop"])
            return lambda x: x[start:stop]
        case "splice":
            start = int(parser.args["start"])
            stop = int(parser.args["stop"])
            insert = parser.args["insert"]
            return lambda x: x[:start] + insert + x[stop:]
        case "append":
            text = parser.args["text"]
            return lambda x: x + text
        case "prepend":
            text = parser.args["text"]
            return lambda x: text + x
        case "startswith":
            text = parser.args["text"]
            return lambda x: x.startswith(text)
        case "endswith":
            text = parser.args["text"]
            return lambda x: x.endswith(text)
        case "contains":
            text = parser.args["text"]
            return lambda x: text in x
        case "trim":
            a = "Hello"
            return lambda x: x.strip()
        ########################################################################
        case "to_year":
            return lambda x: x.year
        case "to_month":
            return lambda x: x.month
        case "to_day":
            return lambda x: x.day
        case "to_weekday":
            return lambda x: x.strftime("%A")
        case "to_hour":
            return lambda x: x.hour
        case "to_minute":
            return lambda x: x.minute
        case "to_second":
            return lambda x: x.second
        case "to_milli":
            return lambda x: x.microsecond / 1000
        case "to_micro":
            return lambda x: x.micorsecond
        case "timedelta":
            year = int(parser.args["year"])
            month = int(parser.args["month"] or "1")
            day = int(parser.args["day"] or "1")
            hour = int(parser.args["hour"] or "0")
            minute = int(parser.args["minute"] or "0")
            second = int(parser.args["second"] or "0")
            microsecond = int(parser.args["microsecond"] or "0")
            return lambda x: x - datetime(
                year, month, day, hour, minute, second, microsecond
            )
        case "unixepoch":
            return lambda x: x.timestamp()

    raise HTTPException(400, f"Unknown parser '{parser.name}'")

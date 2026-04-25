import { Component, computed, signal } from "@angular/core";

import { ParserArgContext, ParserNumberArg, ParserTextArg, ParserUI } from "./parser-arg-types";
import { ParserArgForm } from "./parser-arg-form";
import { AppData } from "../../../../app_data";

@Component({
  selector: "parser-dialog",
  templateUrl: "parsers.html",
  styleUrls: ["../../dialog.css", "parsers.css"],
  imports: [ParserArgForm],
})
export class ParsersDialog {
  tab = signal("types");
  parser = signal("");

  constructor(public appData: AppData) { }

  select(parser: string) {
    this.parser.set(parser);
  }

  parserUI = computed<ParserUI>(() => {
    switch (this.parser()) {
      case "float":
        return new ParserUI({
          name: "float",
          info: "Convert a parameter into a number (floating point number)",
        });
      case "string":
        return new ParserUI({
          name: "string",
          info: "Convert a parameter to text (string)",
        });
      case "datetime":
        return new ParserUI({
          name: "datetime",
          info: "Convert a parameter into a datetime",
          elements: [
            new ParserTextArg({
              key: "format",
              label: "Format",
            }),
          ],
        });
      case "bool":
        return new ParserUI({
          name: "bool",
          info: "Convert a parameter into a boolean",
        });
      case "u8":
        return new ParserUI({
          name: "u8",
          info: "Convert a parameter into an 8-bit unsigned integer",
        });
      case "u16":
        return new ParserUI({
          name: "u16",
          info: "Convert a parameter into a 16-bit unsigned integer",
        });
      case "u32":
        return new ParserUI({
          name: "u32",
          info: "Convert a parameter into a 32-bit unsigned integer",
        });
      case "u64":
        return new ParserUI({
          name: "u64",
          info: "Convert a parameter into a 64-bit unsigned integer",
        });
      case "i8":
        return new ParserUI({
          name: "i8",
          info: "Convert a parameter into an 8-bit signed integer",
        });
      case "i16":
        return new ParserUI({
          name: "i16",
          info: "Convert a parameter into a 16-bit signed integer",
        });
      case "i32":
        return new ParserUI({
          name: "i32",
          info: "Convert a parameter into a 32-bit signed integer",
        });
      case "i64":
        return new ParserUI({
          name: "i64",
          info: "Convert a parameter into a 64-bit signed integer",
        });
      case "f32":
        return new ParserUI({
          name: "f32",
          info: "Convert a parameter into a 32-bit float",
        });
      case "f64":
        return new ParserUI({
          name: "f64",
          info: "Convert a parameter into a 64-bit float",
        });
      case "usize":
        return new ParserUI({
          name: "usize",
          info: "Convert a parameter into a usize (size_t)",
        });
      case "to_scientific":
        return new ParserUI({
          name: "to_scientific",
          info: "Convert a number to scientific notation",
        });
      case "to_hex":
        return new ParserUI({
          name: "to_hex",
          info: "Convert a number into its hexadecimal representation (string)",
        });
      case "from_hex":
        return new ParserUI({
          name: "from_hex",
          info: "Convert a hexadecimal number into a decimal number",
        });
      case "to_bin":
        return new ParserUI({
          name: "to_bin",
          info: "Convert a number into its binary representation (string)",
        });
      case "from_bin":
        return new ParserUI({
          name: "from_bin",
          info: "Convert a binary number into a decimal number",
        });
      case "isfinite":
        return new ParserUI({
          name: "isfinite",
          info: "Check whether a number is finite (not 'nan' and not 'inf')",
        });
      case "isnan":
        return new ParserUI({
          name: "isnan",
          info: "Check whether a number is 'nan'",
        });
      case "isinf":
        return new ParserUI({
          name: "ininf",
          info: "Check whether a number is 'inf'",
        });
      case "add":
        return new ParserUI({
          name: "add",
          info: "Add to a number",
          repr: true,
          elements: [
            new ParserArgContext({
              value: "x + ",
            }),
            new ParserNumberArg({
              key: "number",
            }),
          ],
        });
      case "sub":
        return new ParserUI({
          name: "sub",
          info: "Subtract from a number",
          repr: true,
          elements: [
            new ParserArgContext({
              value: "x - ",
            }),
            new ParserNumberArg({
              key: "number"
            }),
          ],
        });
      case "mul":
        return new ParserUI({
          name: "mul",
          info: "Multiply a number by a given factor",
          repr: true,
          elements: [
            new ParserArgContext({
              value: "x * ",
            }),
            new ParserNumberArg({
              key: "number",
            }),
          ],
        });
      case "div":
        return new ParserUI({
          name: "div",
          info: "Divide a number by a given number",
          repr: true,
          elements: [
            new ParserArgContext({
              value: "x / ",
            }),
            new ParserNumberArg({
              key: "number",
            }),
          ],
        });
      case "round":
        return new ParserUI({
          name: "round",
          info: "Round a number to a given precision",
          elements: [
            new ParserNumberArg({
              key: "decimals",
              label: "Decimals",
            }),
          ],
        });
      case "floor":
        return new ParserUI({
          name: "floor",
          info: "Round a number to the next lower integer",
        });
      case "ceil":
        return new ParserUI({
          name: "ceil",
          info: "Round a number to the next larger integer",
        });
      case "trunc":
        return new ParserUI({
          name: "trunc",
          info: "Get the integer part of a number (round towards zero)",
        });
      case "fract":
        return new ParserUI({
          name: "fract",
          info: "Get the fractional part of a number",
        });
      case "mod":
        return new ParserUI({
          name: "mod",
          info: "Get the remainder of a division (modulo)",
          repr: true,
          elements: [
            new ParserArgContext({
              value: "x mod ",
            }),
            new ParserNumberArg({
              key: "number",
            }),
          ],
        });
      case "floor_div":
        return new ParserUI({
          name: "floor_div",
          info: "Get the whole-number result of a division",
          repr: true,
          elements: [
            new ParserArgContext({
              value: "x // ",
            }),
            new ParserNumberArg({
              key: "number",
            }),
          ],
        });
      case "square":
        return new ParserUI({
          name: "square",
          info: "Square a number",
          repr: true,
          elements: [
            new ParserArgContext({
              value: "x ^ 2",
            }),
          ],
        });
      case "sqrt":
        return new ParserUI({
          name: "sqrt",
          info: "Take the square root of a number",
        });
      case "pow":
        return new ParserUI({
          name: "pow",
          info: "Raise a number to a given power",
          repr: true,
          elements: [
            new ParserArgContext({
              value: "x ^ ",
            }),
            new ParserNumberArg({
              key: "number",
            }),
          ],
        });
      case "exp":
        return new ParserUI({
          name: "exp",
          info: "Get the exponential of a number",
          elements: [
            new ParserNumberArg({
              key: "base",
              label: "Base",
            }),
          ]
        });
      case "log":
        return new ParserUI({
          name: "log",
          info: "Get the logarithm of a number",
          elements: [
            new ParserNumberArg({
              key: "base",
              label: "Base",
            }),
          ]
        });
      case "sin":
        return new ParserUI({
          name: "sin",
          info: "Get the sine of a number",
        });
      case "tan":
        return new ParserUI({
          name: "tan",
          info: "Get the tangent of a number",
        });
      case "arcsin":
        return new ParserUI({
          name: "arcsin",
          info: "Get the inverse sine of a number",
        });
      case "arctan":
        return new ParserUI({
          name: "arctan",
          info: "Get the inverse tangent of a number",
        });
      case "deg":
        return new ParserUI({
          name: "deg",
          info: "Convert a number in radians to degrees",
        });
      case "rad":
        return new ParserUI({
          name: "rad",
          info: "Convert a number in degrees to radians",
        });
      case "sinh":
        return new ParserUI({
          name: "sinh",
          info: "Get the hyperbolic sine of a number",
        });
      case "tanh":
        return new ParserUI({
          name: "tanh",
          info: "Get the hyperbolic tangent of a number",
        });
      case "arsinh":
        return new ParserUI({
          name: "arsinh",
          info: "Get the inverse hyperbolic sine of a number",
        });
      case "artanh":
        return new ParserUI({
          name: "artanh",
          info: "Get the inverse hyperbolic tangent of a number",
        });
      case "erf":
        return new ParserUI({
          name: "erf",
          info: "Get the error function of a number",
        });
      case "gamma":
        return new ParserUI({
          name: "gamma",
          info: "Get the gamma function of a number",
        });
      case "lower":
        return new ParserUI({
          name: "lower",
          info: "Convert all letters in a string to lower case",
        });
      case "upper":
        return new ParserUI({
          name: "upper",
          info: "Convert all letters in a string to upper case",
        });
      case "capitalize":
        return new ParserUI({
          name: "capitalize",
          info: "Convert the first letter in a string to upper case",
        });
      case "title":
        return new ParserUI({
          name: "title",
          info: "Convert the first letter of each word in a string to upper case",
        });
      case "replace":
        return new ParserUI({
          name: "replace",
          info: "Replace a given pattern in a string with another string",
          elements: [
            new ParserTextArg({
              key: "old",
              label: "Old",
            }),
            new ParserTextArg({
              key: "new",
              label: "New",
            }),
            new ParserNumberArg({
              key: "count",
              label: "Count",
            }),
          ]
        });
      case "slice":
        return new ParserUI({
          name: "slice",
          info: "Take out a substring from a string at a given position (including start index, excluding stop index)",
          elements: [
            new ParserNumberArg({
              key: "start",
              label: "Start",
            }),
            new ParserNumberArg({
              key: "stop",
              label: "Stop",
            }),
          ],
        });
      case "splice":
        return new ParserUI({
          name: "splice",
          info: "Remove a substring from a string at a given position, optionally replacing it with another string (including start index, excluding stop index)",
          elements: [
            new ParserNumberArg({
              key: "start",
              label: "Start",
            }),
            new ParserNumberArg({
              key: "stop",
              label: "Stop",
            }),
            new ParserTextArg({
              key: "insert",
              label: "Insert",
            }),
          ]
        });
      case "append":
        return new ParserUI({
          name: "append",
          info: "Append a given string to the end of a string",
          elements: [
            new ParserTextArg({
              key: "text",
              label: "Text",
            }),
          ]
        });
      case "prepend":
        return new ParserUI({
          name: "prepend",
          info: "Prepend a given string to the beginning of a string",
          elements: [
            new ParserTextArg({
              key: "text",
              label: "Text",
            }),
          ],
        });
      case "startswith":
        return new ParserUI({
          name: "startswith",
          info: "Check whether a string starts with a given substring",
          elements: [
            new ParserTextArg({
              key: "text",
              label: "Text",
            }),
          ],
        });
      case "endswith":
        return new ParserUI({
          name: "endswith",
          info: "Check whether a string ends with a given substring",
          elements: [
            new ParserTextArg({
              key: "text",
              label: "Text",
            }),
          ],
        });
      case "contains":
        return new ParserUI({
          name: "contains",
          info: "Check whether a string contains a given substring",
          elements: [
            new ParserTextArg({
              key: "text",
              label: "Text",
            }),
          ],
        });
      case "trim":
        return new ParserUI({
          name: "trim",
          info: "Remove leading and trailing whitespace characters",
        });
      case "to_year":
        return new ParserUI({
          name: "to_year",
          info: "Convert a datetime into its year",
        });
      case "to_month":
        return new ParserUI({
          name: "to_month",
          info: "Convert a datetime into its month (numeric)",
        });
      case "to_day":
        return new ParserUI({
          name: "to_day",
          info: "Convert a datetime into its day (numeric)",
        });
      case "to_weekday":
        return new ParserUI({
          name: "to_weekday",
          info: "Convert a datetime into its weekday (textual)",
        });
      case "to_hour":
        return new ParserUI({
          name: "to_hour",
          info: "Convert a datetime into its hour (24h format)",
        });
      case "to_minute":
        return new ParserUI({
          name: "to_minute",
          info: "Convert a datetime into its minute",
        });
      case "to_second":
        return new ParserUI({
          name: "to_second",
          info: "Convert a datetime into its second",
        });
      case "to_milli":
        return new ParserUI({
          name: "to_milli",
          info: "Convert a datetime into its millisecond",
        });
      case "to_micro":
        return new ParserUI({
          name: "to_micro",
          info: "Convert a datetime into its microsecond",
        });
      case "timedelta":
        return new ParserUI({
          name: "timedelta",
          info: "Get the time difference in seconds to a given datetime",
          elements: [
            new ParserNumberArg({
              key: "year",
              label: "Year",
            }),
            new ParserNumberArg({
              key: "month",
              label: "Month",
            }),
            new ParserNumberArg({
              key: "day",
              label: "Day",
            }),
            new ParserNumberArg({
              key: "hour",
              label: "Hour",
            }),
            new ParserNumberArg({
              key: "minute",
              label: "Minute",
            }),
            new ParserNumberArg({
              key: "second",
              label: "Second",
            }),
            new ParserNumberArg({
              key: "microsecond",
              label: "Microsecond",
            }),
          ],
        });
      case "unixepoch":
        return new ParserUI({
          name: "unixepoch",
          info: "Convert a datetime into seconds since the Unix epoch",
        });
    }
    return new ParserUI({
      info: "Select a parser for more info",
    });
  });
}

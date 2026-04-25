import { Component, signal } from "@angular/core";
import { form, FormField, readonly } from "@angular/forms/signals";
import { parseSelection } from "../../../types";
import { AppData } from "../../../app_data";
import { ParsersDialog } from "./parsers/parsers";
import { Parser } from "./parsers/parser-arg-types";

interface ParseArgs {
  param: string,
  target: "auto" | "DopplerMeasurement" | "SingleSpectrum" | "CoincidenceSpectrum"
  parser: Parser,
}

@Component({
  templateUrl: "parse.html",
  styleUrl: "../dialog.css",
  imports: [FormField, ParsersDialog],
})
export class ParseDialog {
  formModel = signal<ParseArgs>({
    param: "",
    target: "auto",
    parser: {
      name: "",
      args: {},
      repr: "",
    },
  });

  args = form(this.formModel, (schemaPath) => {
    readonly(schemaPath.parser);
  });

  constructor(public appData: AppData) { }

  close() {
    this.appData.dialogOpen.set(false);
  }

  openParserDialog() {
    this.appData.parsersOpen.set(true);
  }

  addParser(parser: Parser) {
    this.formModel.update(model => ({ ...model, parser: parser }));
  }

  async submit(event: Event) {
    event.preventDefault();

    const payload = {
      selection: parseSelection(this.appData.selection()),
      args: {
        params: [this.args.param().value()],
        targets: [this.args.target().value()],
        parsers: [this.args.parser().value()],
      }
    };

    const response = await fetch("http://127.0.0.1:8000/parse", {
      method: "POST",
      headers: {
        "Content-Type": "application/json"
      },
      body: JSON.stringify(payload),
    });

    if (response.ok) {
      this.close();
    } else {
      console.error(response);
    }
  }
}

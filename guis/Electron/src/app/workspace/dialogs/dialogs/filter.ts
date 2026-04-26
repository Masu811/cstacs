import { Component, effect, inject } from "@angular/core";
import { FormArray, FormBuilder, ReactiveFormsModule, Validators } from "@angular/forms";
import { Toggle } from "./toggle";
import { AppData } from "../../../app_data";
import { MultiCampaign, parseSelection } from "../../../types";
import { Parser } from "./parsers/parser-arg-types";

type Arg = {
  param: string,
  target: "auto" | "DopplerMeasurement" | "SingleSpectrum" | "CoincidenceSpectrum",
  parser: Parser,
  byValue: boolean,
  values: string[],
  min: number,
  max: number,
  negative: boolean,
}

@Component({
  templateUrl: "filter.html",
  styleUrl: "../dialog.css",
  imports: [ReactiveFormsModule, Toggle],
})
export class FilterDialog {
  private formBuilder = inject(FormBuilder);

  formModel = this.formBuilder.group({
    param: ['', Validators.required],
    target: ['auto'],
    parser: [{
      name: "",
      args: {},
      repr: "",
    }],
    byValue: [true],
    values: this.formBuilder.array([this.formBuilder.control('')]),
    min: [null],
    max: [null],
    negative: [false],
  });

  get values() {
    return this.formModel.get('values') as FormArray;
  }

  addValue() {
    this.values.push(this.formBuilder.control(''));
  }

  constructor(public appData: AppData) { }

  close() {
    this.appData.dialogOpen.set(false);
  }

  async openParserDialog() {
    const parser = await this.appData.openParserDialog();
    if (parser === null) return;
    this.formModel.patchValue({ parser: parser });
  }

  async submit(event: Event) {
    event.preventDefault();

    const args = this.formModel.getRawValue();

    if (args.min == null) {
      //@ts-ignore
      args.min = "-inf";
    }
    if (args.max == null) {
      //@ts-ignore
      args.max = "inf";
    }

    const payload = {
      selection: parseSelection(this.appData.selection()),
      args: args,
    };

    const response = await fetch("http://127.0.0.1:8000/filter", {
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
      return;
    }

    const newData = await response.json() as Array<MultiCampaign>;

    this.appData.data.set(newData);
  }
}

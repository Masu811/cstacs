import { Component, inject } from "@angular/core";
import { FormArray, FormBuilder, ReactiveFormsModule } from "@angular/forms";
import { AppData } from "../../../app_data";
import { parseSelection } from "../../../types";
import { Router } from "@angular/router";
import { Parser } from "./parsers/parser-arg-types";

type Param = {
  param: string,
  target: "auto" | "DopplerMeasurement" | "SingleSpectrum" | "CoincidenceSpectrum",
  parser: Parser,
}

@Component({
  templateUrl: "plot.html",
  styleUrl: "../dialog.css",
  imports: [ReactiveFormsModule],
})
export class PlotDialog {
  private formBuilder = inject(FormBuilder);

  formModel = this.formBuilder.group({
    x_elements: this.formBuilder.array([
      this.newParam(),
    ]),
    y_elements: this.formBuilder.array([
      this.newParam(),
    ]),
    z_elements: this.formBuilder.array([
      this.newParam(),
    ]),
    errorbars: [true],
    scatter: [false],
    filled: [false],
    label: [""],
    label_is_param: [false],
    detname_in_label: [null],
  });

  private newParam() {
    return this.formBuilder.group({
      param: "",
      target: "auto",
      parser: {
        name: "",
        args: {},
        repr: "",
      },
    } as Param);
  }

  get xParams() {
    return this.formModel.get('x_elements') as FormArray;
  }

  addxParam() {
    this.xParams.push(this.newParam());
  }

  get yParams() {
    return this.formModel.get('y_elements') as FormArray;
  }

  addyParam() {
    this.yParams.push(this.newParam());
  }

  get zParams() {
    return this.formModel.get('z_elements') as FormArray;
  }

  addzParam() {
    this.zParams.push(this.newParam());
  }

  validNumOfParams() {
    const lx = this.xParams.length;
    const ly = this.yParams.length;
    const lz = this.zParams.length;
    if (lx > 1) {
      if (ly > 1 && lx != ly) {
        return false;
      }
      if (lz > 1 && lx != lz) {
        return false;
      }
    }
    if (ly > 1) {
      if (lz > 1 && ly != lz) {
        return false;
      }
    }
    return true;
  }

  constructor(public appData: AppData, private router: Router) { }

  close() {
    this.appData.dialogOpen.set(false);
  }

  async openParserDialog(dim: string, i: number) {
    const parser = await this.appData.openParserDialog();
    if (parser === null) return;
    switch (dim) {
      case "x":
        this.xParams.at(i).patchValue({ parser: parser })
        break;
      case "y":
        this.yParams.at(i).patchValue({ parser: parser })
        break;
      case "z":
        this.zParams.at(i).patchValue({ parser: parser })
        break;
    }
  }

  async submit(event: Event) {
    event.preventDefault();

    const payload = {
      selection: parseSelection(this.appData.selection()),
      args: this.formModel.getRawValue(),
    };

    console.log(payload);

    const response = await fetch("http://127.0.0.1:8000/plot", {
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

    const plotData = await response.json();

    // plotData: Array<{[fieldname: string]: [values: Array<number>]}



    console.log(plotData);

    this.appData.plotData.set(plotData);
    this.router.navigate(["/plots"]);
  }
}

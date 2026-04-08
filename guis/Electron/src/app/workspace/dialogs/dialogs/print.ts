import { Component, inject } from "@angular/core";
import { ReactiveFormsModule, FormBuilder, FormArray } from "@angular/forms";
import { parseSelection } from "../../../types";
import { AppData } from "../../../services/app_data";
import { Router } from "@angular/router";

type Param = {
  param: string,
  target: "DopplerMeasurement" | "SingleSpectrum" | "CoincidenceSpectrum",
  parser: string,
}

interface PrintArgs {
  params: Array<Param>,
}

@Component({
  templateUrl: "print.html",
  styleUrl: "../dialog.css",
  imports: [ReactiveFormsModule]
})
export class PrintDialog {
  private FormBuilder = inject(FormBuilder);

  formModel = this.FormBuilder.group({
    items: this.FormBuilder.array([this.createItem()])
  });

  createItem() {
    return this.FormBuilder.group({
      param: "",
      target: "DopplerMeasurement",
      parser: "",
    } as Param);
  }

  get items() {
    return this.formModel.get("items") as FormArray;
  }

  addItem() {
    this.items.push(this.createItem());
  }

  constructor(public appData: AppData, private router: Router) { }

  close() {
    this.appData.dialogOpen.set(false);
  }

  openParserDialog() { }

  async submit(event: Event) {
    event.preventDefault();

    const items = this.items.controls.map(control => control.value);

    const payload = {
      selection: parseSelection(this.appData.selection()),
      args: {
        params: items.map(item => item.param),
        targets: items.map(item => item.target),
        parsers: items.map(item => item.parser),
      }
    };

    const response = await fetch("http://127.0.0.1:8000/print", {
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

    const data = await response.json();

    this.appData.tableData.set(data);
    this.router.navigate(["/table"]);
  }
}

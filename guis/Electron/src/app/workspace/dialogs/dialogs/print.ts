import { Component, effect, inject } from "@angular/core";
import { ReactiveFormsModule, FormBuilder, FormArray, FormGroup } from "@angular/forms";
import { parseSelection } from "../../../types";
import { AppData } from "../../../app_data";
import { Router } from "@angular/router";
import { Parser } from "./parsers/parser-arg-types";

type Param = {
  param: string,
  target: "auto" | "DopplerMeasurement" | "SingleSpectrum" | "CoincidenceSpectrum",
  parser: Parser,
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
      target: "auto",
      parser: {
        name: "",
        args: {},
        repr: "",
      },
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

  openParserDialog(i: number) {
    this.appData.parserIndex = i;
    this.appData.parsersOpen.set(true);
  }

  addParserEffect = effect(() => {
    const i = this.appData.parserIndex;
    const parser = this.appData.parserSelected();
    const group = this.items.at(i) as FormGroup;
    group.get("parser")?.setValue(parser);
  });

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

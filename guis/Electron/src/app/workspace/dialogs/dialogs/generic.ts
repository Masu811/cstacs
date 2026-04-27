import { inject } from "@angular/core";
import { FormBuilder, FormArray, FormGroup } from "@angular/forms";
import { Router } from "@angular/router";

import { parseSelection } from "../../../types";
import { AppData } from "../../../app_data";
import { Parser } from "./parsers/parser-arg-types";

type Param = {
  param: string,
  target: "auto" | "DopplerMeasurement" | "SingleSpectrum" | "CoincidenceSpectrum",
  parser: Parser,
}

export abstract class GenericCampaignDialog {
  abstract title: string;
  abstract endpoint: string;

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

  constructor(public appData: AppData, protected router: Router) { }

  close() {
    this.appData.dialogOpen.set(false);
  }

  async openParserDialog(i: number) {
    const parser = await this.appData.openParserDialog();
    if (parser === null) return;
    const group = this.items.at(i) as FormGroup;
    group.patchValue({ parser: parser });
  }

  payload() {
    const items = this.items.controls.map(control => control.value);

    return {
      selection: parseSelection(this.appData.selection()),
      args: {
        params: items.map(item => item.param),
        targets: items.map(item => item.target),
        parsers: items.map(item => item.parser),
      }
    };
  }

  abstract handleResponse(response: Response): Promise<void>;

  async submit(event: Event): Promise<void> {
    event.preventDefault();

    const payload = this.payload();

    const response = await fetch(this.endpoint, {
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

    this.handleResponse(response);
  }
}

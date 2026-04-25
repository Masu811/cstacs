import { Component, input, output } from "@angular/core";
import { Dtype, DtypeCounter } from "../../../../../types";
import { AppData } from "../../../../../app_data";

@Component({
  selector: "datatypes",
  templateUrl: "datatypes.html",
  styleUrl: "datatypes.css",
})
export class Datatypes {
  openCounter = input.required<DtypeCounter>();
  dtypeToggle = output<Dtype>();

  constructor(public appData: AppData) { }

  toggleDtypeDisplay(type: Dtype) {
    this.dtypeToggle.emit(type);
  }
}

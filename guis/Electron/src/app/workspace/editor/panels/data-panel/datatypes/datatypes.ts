import { Component, input, model } from "@angular/core";
import { Dtype, DtypeCounter, Selection } from "../../../../../types";

@Component({
  selector: "datatypes",
  templateUrl: "datatypes.html",
  styleUrl: "datatypes.css",
})
export class Datatypes {
  dtypes = Dtype;
  availDtypes = input.required<Selection>();
  openCounter = model.required<DtypeCounter>();
  dtypeDisplayLevel = model(0);

  toggleDtypeDisplay(type: Dtype) {
    this.dtypeDisplayLevel.set(type);
  }
}

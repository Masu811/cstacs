import { Component, input, model, output } from "@angular/core";
import { Dtype, DtypeCounter, DtypeToggle } from "../../../../../types";

@Component({
  selector: "datatypes",
  templateUrl: "datatypes.html",
  styleUrl: "datatypes.css",
})
export class Datatypes {
  dtypes = Dtype;
  availDtypes = input.required<DtypeToggle>();
  openCounter = input.required<DtypeCounter>();
  dtypeToggle = output<Dtype>();

  toggleDtypeDisplay(type: Dtype) {
    this.dtypeToggle.emit(type);
  }
}

import { Component, inject } from "@angular/core";
import { ReactiveFormsModule, FormBuilder, FormArray } from "@angular/forms";
import { parseSelection } from "../../../types";
import { AppData } from "../../../services/app_data";

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

  constructor(public appData: AppData) { }

  close() {
    this.appData.dialogOpen.set(false);
  }

  openParserDialog() { }

  submit(event: Event) {
    event.preventDefault();

    console.log(this.items.controls[0].value);
  }

  // async submit(event: Event) {
  //   event.preventDefault();

  //   const payload = {
  //     selection: parseSelection(this.appData.selection()),
  //     args: {
  //       s_width: this.args.s_width().value(),
  //       w_width: this.args.w_width().value(),
  //       w_dist: this.args.w_dist().value(),
  //       w_rightonly: this.args.w_rightonly().value(),
  //       peak_width: this.args.peak_width().value(),
  //       bg_frac: this.args.bg_frac().value(),
  //       bg_corr: this.args.bg_corr().value(),
  //       v2p_bounds: [
  //         this.args.v2p_valley_lower().value(),
  //         this.args.v2p_valley_upper().value(),
  //         this.args.v2p_peak_lower().value(),
  //         this.args.v2p_peak_upper().value(),
  //       ],
  //       follow_peak_order: this.args.follow_peak_order().value(),
  //     }
  //   };

  //   const response = await fetch("http://127.0.0.1:8000/single_analyze", {
  //     method: "POST",
  //     headers: {
  //       "Content-Type": "application/json"
  //     },
  //     body: JSON.stringify(payload),
  //   });

  //   if (response.ok) {
  //     this.close();
  //   } else {
  //     console.error(response);
  //   }
  // }
}

import { Component, signal } from "@angular/core";
import { form, FormField, max, min } from "@angular/forms/signals";
import { parseSelection } from "../../../types";
import { AppData } from "../../../services/app_data";

interface SingleAnalyzeArgs {
  s_width: number;
  w_width: number;
  w_dist: number;
  w_rightonly: boolean;
  peak_width: number;
  bg_corr: boolean;
  bg_frac: number;
  v2p_valley_lower: number;
  v2p_valley_upper: number;
  v2p_peak_lower: number;
  v2p_peak_upper: number;
  follow_peak_order: string;
}

@Component({
  templateUrl: "single_analyze.html",
  styleUrl: "../dialog.css",
  imports: [FormField],
})
export class SingleAnalyzeDialog {
  formModel = signal<SingleAnalyzeArgs>({
    s_width: 1.1,
    w_width: 1.0,
    w_dist: 3.0,
    w_rightonly: true,
    peak_width: 60.0,
    bg_corr: true,
    bg_frac: 0.25,
    v2p_valley_lower: 450.0,
    v2p_valley_upper: 490.0,
    v2p_peak_lower: 506.0,
    v2p_peak_upper: 516.0,
    follow_peak_order: "1",
  });

  args = form(this.formModel, (schemaPath) => {
    min(schemaPath.s_width, 0);
    min(schemaPath.w_width, 0);
    min(schemaPath.w_dist, 0);
    min(schemaPath.peak_width, 0);
    min(schemaPath.bg_frac, 0);
    max(schemaPath.bg_frac, 1);
    min(schemaPath.v2p_valley_lower, 0);
    min(schemaPath.v2p_valley_upper, 0);
    min(schemaPath.v2p_peak_lower, 0);
    min(schemaPath.v2p_peak_upper, 0);
  });

  constructor(public appData: AppData) { }

  close() {
    this.appData.dialogOpen.set(false);
  }

  async submit(event: Event) {
    event.preventDefault();

    const payload = {
      selection: parseSelection(this.appData.selection()),
      args: {
        s_width: this.args.s_width().value(),
        w_width: this.args.w_width().value(),
        w_dist: this.args.w_dist().value(),
        w_rightonly: this.args.w_rightonly().value(),
        peak_width: this.args.peak_width().value(),
        bg_frac: this.args.bg_frac().value(),
        bg_corr: this.args.bg_corr().value(),
        v2p_bounds: [
          this.args.v2p_valley_lower().value(),
          this.args.v2p_valley_upper().value(),
          this.args.v2p_peak_lower().value(),
          this.args.v2p_peak_upper().value(),
        ],
        follow_peak_order: this.args.follow_peak_order().value(),
      }
    };

    const response = await fetch("http://127.0.0.1:8000/single_analyze", {
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

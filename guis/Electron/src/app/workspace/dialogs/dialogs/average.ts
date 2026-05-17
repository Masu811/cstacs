import { Component } from "@angular/core";
import { ReactiveFormsModule } from "@angular/forms";
import { Router } from "@angular/router";

import { AppData } from "../../../app_data";
import { AggregateDialog } from "./aggregate";

@Component({
  templateUrl: "aggregate.html",
  styleUrl: "../dialog.css",
  imports: [ReactiveFormsModule],
})
export class AverageDialog extends AggregateDialog {
  override title = "MeasurementCampaign.average";
  override endpoint = "http://127.0.0.1:8000/average";

  constructor(appData: AppData, router: Router) {
    super(appData, router);
  }
}

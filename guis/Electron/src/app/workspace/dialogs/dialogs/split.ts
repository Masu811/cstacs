import { Component } from "@angular/core";
import { ReactiveFormsModule } from "@angular/forms";
import { Router } from "@angular/router";

import { GenericCampaignDialog } from "./generic";
import { AppData } from "../../../app_data";
import { MultiCampaign } from "../../../types";

@Component({
  templateUrl: "generic.html",
  styleUrl: "../dialog.css",
  imports: [ReactiveFormsModule]
})
export class SplitDialog extends GenericCampaignDialog {
  override title = "MeasurementCampaign.split";
  override endpoint = "http://127.0.0.1:8000/split";

  constructor(appData: AppData, router: Router) {
    super(appData, router);
  }

  override async handleResponse(response: Response) {
    const newData = await response.json() as Array<MultiCampaign>;
    this.appData.deselect.update(val => !val);
    this.appData.data.set(newData);
    this.appData.clearSelection();
  }
}

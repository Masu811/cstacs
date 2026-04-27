import { Component } from "@angular/core";
import { ReactiveFormsModule } from "@angular/forms";
import { Router } from "@angular/router";

import { GenericCampaignDialog } from "./generic";
import { AppData } from "../../../app_data";

@Component({
  templateUrl: "generic.html",
  styleUrl: "../dialog.css",
  imports: [ReactiveFormsModule]
})
export class PrintDialog extends GenericCampaignDialog {
  override title = "MeasurementCampaign.print";
  override endpoint = "http://127.0.0.1:8000/print";

  constructor(appData: AppData, router: Router) {
    super(appData, router);
  }

  override async handleResponse(response: Response) {
    const data = await response.json();

    this.appData.tableData.set(data);
    this.router.navigate(["/table"]);
  }
}

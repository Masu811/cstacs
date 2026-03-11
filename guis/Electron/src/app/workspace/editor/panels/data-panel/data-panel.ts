import { Component, model, ModelSignal } from "@angular/core";
import { MultiCampaign } from "../../../../types";

@Component({
  selector: "data-panel",
  templateUrl: "data-panel.html",
  styleUrl: "data-panel.css",
})
export class DataPanel {
  data: ModelSignal<Array<MultiCampaign>> = model(Array<MultiCampaign>());
}

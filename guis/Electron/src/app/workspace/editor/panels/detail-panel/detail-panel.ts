import { Component, model, ModelSignal } from "@angular/core";
import { Panel } from "../panel";
import { MultiCampaign } from "../../../../types";

@Component({
  selector: "detail-panel",
  templateUrl: "detail-panel.html",
  styleUrls: ["../panel.css", "detail-panel.css"],
})
export class DetailPanel extends Panel {
  data: ModelSignal<Array<MultiCampaign>> = model(Array<MultiCampaign>());
}

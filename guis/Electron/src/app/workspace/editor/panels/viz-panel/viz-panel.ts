import { Component, model, ModelSignal } from "@angular/core";
import { Panel } from "../panel";
import { MultiCampaign } from "../../../../types";

@Component({
  selector: "viz-panel",
  templateUrl: "viz-panel.html",
  styleUrls: ["../panel.css", "viz-panel.css"],
})
export class VizPanel extends Panel {
  data: ModelSignal<Array<MultiCampaign>> = model(Array<MultiCampaign>());
}

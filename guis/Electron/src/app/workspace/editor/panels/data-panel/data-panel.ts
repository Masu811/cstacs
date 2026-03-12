import { Component, model, ModelSignal } from "@angular/core";
import { Panel } from "../panel";
import { Dropdown } from "./dropdown/dropdown";
import { MultiCampaign } from "../../../../types";

@Component({
  selector: "data-panel",
  templateUrl: "data-panel.html",
  styleUrls: ["../panel.css", "data-panel.css"],
  imports: [Dropdown],
})
export class DataPanel extends Panel {
  data: ModelSignal<Array<MultiCampaign>> = model(Array<MultiCampaign>());
}

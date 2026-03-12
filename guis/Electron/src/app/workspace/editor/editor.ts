import { Component, model, ModelSignal } from "@angular/core";
import { MultiCampaign } from "../../types";
import { DataPanel } from "./panels/data-panel/data-panel";
import { DetailPanel } from "./panels/detail-panel/detail-panel";
import { VizPanel } from "./panels/viz-panel/viz-panel";

@Component({
  selector: "editor",
  templateUrl: "editor.html",
  styleUrl: "editor.css",
  imports: [DataPanel, DetailPanel, VizPanel],
})
export class Editor {
  projectLoaded = model(false);
  data: ModelSignal<Array<MultiCampaign>> = model(Array<MultiCampaign>());
}

import { Component, model, ModelSignal, signal } from "@angular/core";
import { MultiCampaign } from "../../types";
import { DataPanel } from "./panels/data-panel/data-panel";
import { MetadataPanel } from "./panels/metadata-panel/metadata-panel";
import { VizPanel } from "./panels/viz-panel/viz-panel";

@Component({
  selector: "editor",
  templateUrl: "editor.html",
  styleUrl: "editor.css",
  imports: [DataPanel, MetadataPanel, VizPanel],
})
export class Editor {
  projectLoaded = model(false);
  data: ModelSignal<Array<MultiCampaign>> = model(Array<MultiCampaign>());
  details = signal({});
}

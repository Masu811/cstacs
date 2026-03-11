import { Component, model, ModelSignal } from "@angular/core";
import { MultiCampaign } from "../../types";
import { DataPanel } from "./panels/data-panel/data-panel";

@Component({
  selector: "editor",
  templateUrl: "editor.html",
  styleUrl: "editor.css",
  imports: [DataPanel],
})
export class Editor {
  projectLoaded = model(false);
  data: ModelSignal<Array<MultiCampaign>> = model(Array<MultiCampaign>());
}

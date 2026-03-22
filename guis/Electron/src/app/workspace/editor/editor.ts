import { Component, model, input } from "@angular/core";

import { MultiCampaign, DtypeToggle, DtypeCounter } from "../../types";
import { Welcome } from "./panels/welcome-panel/welcome";
import { DataPanel } from "./panels/data-panel/data-panel";
import { DisplayPanel } from "./panels/display-panel/display-panel";
import { ResizeDirective } from "./resize-handle";

@Component({
  selector: "editor",
  templateUrl: "editor.html",
  styleUrl: "editor.css",
  imports: [Welcome, DataPanel, DisplayPanel, ResizeDirective],
})
export class Editor {
  projectLoaded = model(false);
  data = model(Array<MultiCampaign>());

  availDtypes = input.required<DtypeToggle>();

  selection = model.required<DtypeCounter>();
}

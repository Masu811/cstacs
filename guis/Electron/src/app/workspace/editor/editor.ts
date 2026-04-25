import { Component } from "@angular/core";

import { Welcome } from "./panels/welcome-panel/welcome";
import { DataPanel } from "./panels/data-panel/data-panel";
import { DisplayPanel } from "./panels/display-panel/display-panel";
import { ResizeDirective } from "./resize-handle";
import { AppData } from "../../app_data";

@Component({
  selector: "editor",
  templateUrl: "editor.html",
  styleUrl: "editor.css",
  imports: [Welcome, DataPanel, DisplayPanel, ResizeDirective],
})
export class Editor {
  constructor(public appData: AppData) { }
}

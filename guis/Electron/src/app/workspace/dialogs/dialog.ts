import { Component, signal } from "@angular/core";
import { NgComponentOutlet } from "@angular/common";

import { SingleAnalyzeDialog } from "./dialogs/single_analyze";

@Component({
  selector: "custom-dialog",
  templateUrl: "dialog.html",
  styleUrl: "dialog.css",
  imports: [NgComponentOutlet],
})
export class Dialog {
  activeDialog = signal(SingleAnalyzeDialog);
}

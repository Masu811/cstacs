import { Component, computed, Type } from "@angular/core";
import { NgComponentOutlet } from "@angular/common";

import { AppData } from "../../app_data";
import { SingleAnalyzeDialog } from "./dialogs/single_analyze";
import { PrintDialog } from "./dialogs/print";
import { ParseDialog } from "./dialogs/parse";

@Component({
  selector: "custom-dialog",
  templateUrl: "dialog.html",
  styleUrl: "dialog.css",
  imports: [NgComponentOutlet],
})
export class Dialog {
  activeDialog = computed<Type<any> | null>(() => {
    switch (this.appData.dialogType()) {
      case "singleAnalyze":
        return SingleAnalyzeDialog;
      case "print":
        return PrintDialog;
      case "parse":
        return ParseDialog;
      default:
        return null;
    }
  });

  constructor(public appData: AppData) { }
}

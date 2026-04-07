import { Component, computed, signal, Type } from "@angular/core";
import { NgComponentOutlet } from "@angular/common";

import { SingleAnalyzeDialog } from "./dialogs/single_analyze";
import { PrintDialog } from "./dialogs/print";
import { AppData } from "../../services/app_data";

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
      default:
        return null;
    }
  });

  constructor(public appData: AppData) { }
}

import { Component, computed, Type } from "@angular/core";
import { NgComponentOutlet } from "@angular/common";

import { AppData } from "../../app_data";
import { SingleAnalyzeDialog } from "./dialogs/single_analyze";
import { PrintDialog } from "./dialogs/print";
import { ParseDialog } from "./dialogs/parse";
import { FilterDialog } from "./dialogs/filter";
import { SortDialog } from "./dialogs/sort";
import { SplitDialog } from "./dialogs/split";
import { PlotDialog } from "./dialogs/plot";

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
      case "filter":
        return FilterDialog;
      case "sort":
        return SortDialog;
      case "split":
        return SplitDialog;
      case "plot":
        return PlotDialog;
      default:
        return null;
    }
  });

  constructor(public appData: AppData) { }
}

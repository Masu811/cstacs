import { Component, input, signal, model, computed  } from "@angular/core";
import { NgComponentOutlet } from "@angular/common";

import { SingleAnalyzeDialog } from "./dialogs/single_analyze";
import { DtypeSelection } from "../../types";

@Component({
  selector: "custom-dialog",
  templateUrl: "dialog.html",
  styleUrl: "dialog.css",
  imports: [NgComponentOutlet],
})
export class Dialog {
  dialogType = input.required<string>();
  dialogOpen = model<boolean>(false);
  selection = input.required<DtypeSelection>();

  activeDialog = signal(SingleAnalyzeDialog);

  dialogInputs = computed(() => {
    return {
      dialogOpen: this.dialogOpen,
      selection: this.selection(),
    };
  });
}

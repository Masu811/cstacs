import { Component, input, signal, model, computed  } from "@angular/core";
import { NgComponentOutlet } from "@angular/common";

import { SingleAnalyzeDialog } from "./dialogs/single_analyze";

@Component({
  selector: "custom-dialog",
  templateUrl: "dialog.html",
  styleUrl: "dialog.css",
  imports: [NgComponentOutlet],
})
export class Dialog {
  dialogType = input.required<string>();
  dialogOpen = model<boolean>(false);

  activeDialog = signal(SingleAnalyzeDialog);

  dialogInputs = computed(() => {
    return {
      dialogOpen: this.dialogOpen(),
    };
  });
}

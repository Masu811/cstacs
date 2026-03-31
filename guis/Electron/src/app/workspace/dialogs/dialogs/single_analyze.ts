import { Component, input, ModelSignal, model } from "@angular/core";

@Component({
  templateUrl: "single_analyze.html",
  styleUrl: "../dialog.css",
})
export class SingleAnalyzeDialog {
  dialogOpen = input.required<boolean>();

  constructor() {
    setInterval(() => console.log(this.dialogOpen()), 1000);
  }

  // async analyzeSingles() {
  //   const payload = {
  //     selection: this.parseSelection(),
  //     // args:
  //   };

  //   const response = await fetch("http://127.0.0.1:8000/single_analyze", {
  //     method: "POST",
  //     headers: {
  //       "Content-Type": "application/json"
  //     },
  //     body: JSON.stringify(payload),
  //   });
  // }
}

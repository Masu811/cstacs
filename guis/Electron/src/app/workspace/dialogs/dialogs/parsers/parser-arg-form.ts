import { Component, input } from "@angular/core";
import { ReactiveFormsModule } from "@angular/forms";
import { ParserUI } from "./parser-arg-types";
import { AppData } from "../../../../app_data";

@Component({
  selector: "parser-arg-form",
  templateUrl: "parser-arg-form.html",
  styleUrls: ["../../dialog.css", "parser-arg-form.css"],
  imports: [ReactiveFormsModule],
})
export class ParserArgForm {
  parserUI = input.required<ParserUI>();

  constructor(public appData: AppData) { }

  close() {
    this.appData.parsersOpen.set(false);
  }

  submit() {
    this.appData.parserSelected.set(this.parserUI().toPayload());
    this.close();
  }
}

import { Component, input, output, signal, model } from "@angular/core";
import { Dtype, DtypeCounter } from "../../../../../types";

@Component({
  selector: "dropdown",
  templateUrl: "dropdown.html",
  styleUrls: ["../data-panel.css", "dropdown.css"],
})
export class Dropdown {
  name = input("unnamed");
  kind = input.required<Dtype>();
  icon = input("");
  open = signal(false);
  clicked = output();
  openCounter = model.required<DtypeCounter>();

  clickHandler() {
    this.toggleOpen();
    this.clicked.emit();
  }

  toggleOpen() {
    this.open.update(val => !val);
    this.openCounter.update(oldCounter => {
      return {
        ...oldCounter,
        [this.kind()]: oldCounter[this.kind()] + (this.open() ? 1 : -1),
      };
    });
  }

  handleDtypeToggle() {

  }
}

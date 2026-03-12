import { Component, input, output, signal } from "@angular/core";

@Component({
  selector: "dropdown",
  templateUrl: "dropdown.html",
  styleUrl: "dropdown.css",
})
export class Dropdown {
  name = input("unnamed");
  icon = input("");
  open = signal(false);
  clicked = output();

  clickHandler() {
    this.toggleOpen();
    this.clicked.emit();
  }

  toggleOpen() {
    this.open.update(val => !val);
  }
}

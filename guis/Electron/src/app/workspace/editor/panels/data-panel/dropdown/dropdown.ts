import { Component, input, signal } from "@angular/core";

@Component({
  selector: "dropdown",
  templateUrl: "dropdown.html",
  styleUrl: "dropdown.css",
})
export class Dropdown {
  name = input("unnamed");
  icon = input("");
  open = signal(false);

  toggleOpen() {
    this.open.update(val => !val);
  }
}

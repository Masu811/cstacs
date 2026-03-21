import { Component, input, signal, effect, SkipSelf } from "@angular/core";
import { Dtype } from "../../../../../types";
import { Dropdown } from "../dropdown/dropdown";

@Component({
  selector: "leaf",
  templateUrl: "leaf.html",
  styleUrls: ["../data-panel.css", "leaf.css"],
})
export class Leaf {
  name = input("unnamed");
  icon = input("");
  kind = input.required<Dtype>();
  dtypes = Dtype;

  parentSelectEffect = effect(() => {
    if (this.parent.selected()) {
      this.selected.set(true);
    } else if (this.parent.manuallySelected) {
      this.manuallySelected = true;
      this.selected.set(false);
    }
  });
  childSelectEffect = effect(() => {
    if (!this.selected() && !this.manuallySelected) {
      this.parent.manuallySelected = false;
      this.parent.selected.set(false);
    }
  });
  manuallySelected = false;
  selected = signal(false);

  constructor(@SkipSelf() private parent: Dropdown) { }

  toggleSelect() {
    this.manuallySelected = true;
    this.selected.update(val => !val);
    if (this.parent.selected() && !this.selected()) {
      this.parent.manuallySelected = false;
      this.parent.selected.set(false);
    }
  }
}

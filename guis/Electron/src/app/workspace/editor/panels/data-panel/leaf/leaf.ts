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
    this.selected.set(this.parent.selected());
  })
  selected = signal(false);

  constructor(@SkipSelf() private parent: Dropdown) {
    this.parent = parent;
  }

  toggleSelect() {
    this.selected.update(val => !val);
  }
}

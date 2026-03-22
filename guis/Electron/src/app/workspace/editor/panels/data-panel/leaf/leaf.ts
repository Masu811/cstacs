import { Component, input, signal, effect, SkipSelf, model } from "@angular/core";
import { Dtype, DtypeCounter } from "../../../../../types";
import { Dropdown } from "../dropdown/dropdown";

@Component({
  selector: "leaf",
  templateUrl: "leaf.html",
  styleUrls: ["../data-panel.css", "leaf.css"],
})
export class Leaf {
  name = input("unnamed");
  icon = input("");
  kind = model.required<Dtype>();
  dtypes = Dtype;

  selection = model.required<DtypeCounter>();

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
  selectEffect = effect(() => {
    this.selection.update(oldSelection => {
      const oldValue = oldSelection[this.kind()];
      return {
        ...oldSelection,
        [this.kind()]: Math.max(0, oldValue + (this.selected() ? 1 : -1)),
      };
    });
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

import { Component, input, signal, effect, SkipSelf, model } from "@angular/core";
import { Dtype, DtypeCounter, DtypeSelection } from "../../../../../types";
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
  id = input.required<string>();

  selection = model.required<DtypeSelection>();
  deselect = input.required<boolean>();

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
    const isSelected = this.selected();
    const id = this.id();
    const kind = this.kind();

    this.selection.update(old => {
      const set = new Set(old[kind]);

      if (isSelected) set.add(id);
      else set.delete(id);

      return {
        ...old,
        [kind]: set,
      };
    });
  });
  deselectEffect = effect(() => {
    this.deselect();
    this.selected.set(false);
  })
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

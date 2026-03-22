import { Component, input, signal, model, effect, SkipSelf, Optional } from "@angular/core";
import { Dtype, DtypeCounter } from "../../../../../types";

@Component({
  selector: "dropdown",
  templateUrl: "dropdown.html",
  styleUrls: ["../data-panel.css", "dropdown.css"],
})
export class Dropdown {
  name = input("unnamed");
  icon = input("");
  kind = input.required<Dtype>();

  selection = model.required<DtypeCounter>();

  openCounter = model.required<DtypeCounter>();
  open = signal(false);
  counterEffect = effect(() => {
    this.openCounter.update(oldCounter => {
      return {
        ...oldCounter,
        [this.kind()]: Math.max(0, oldCounter[this.kind()] + (this.open() ? 1 : -1)),
      };
    });
  })
  globalToggle = input([false]);
  globalToggleEffect = effect(() => {
    const state = this.globalToggle()[0];
    this.open.set(state);
    this.localToggle.set(state);
  });
  localToggle = signal(false);
  localToggleEffect = effect(() => {
    this.open.set(this.localToggle());
  });

  parentSelectEffect = effect(() => {
    if (!this.parent) return;

    if (this.parent.selected()) {
      this.selected.set(true);
    } else if (this.parent.manuallySelected) {
      this.manuallySelected = true;
      this.selected.set(false);
    }
  });
  childSelectEffect = effect(() => {
    if (!this.parent) return;

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

  constructor(@Optional() @SkipSelf() private parent: Dropdown) { }

  toggleOpen() {
    this.localToggle.update(val => !val);
  }

  toggleSelect() {
    this.manuallySelected = true;
    this.selected.update(val => !val);
    if (this.parent?.selected() && !this.selected()) {
      this.parent.manuallySelected = false;
      this.parent.selected.set(false);
    }
  }
}

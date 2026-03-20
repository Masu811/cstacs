import { Component, input, signal, model, effect, SkipSelf, Optional } from "@angular/core";
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
    if (this.parent) {
      this.selected.set(this.parent.selected());
    }
  })
  selected = signal(false);

  constructor(@Optional() @SkipSelf() private parent: Dropdown) {
    this.parent = parent;
  }

  toggleOpen() {
    this.localToggle.update(val => !val);
  }

  toggleSelect() {
    this.selected.update(val => !val);
  }
}

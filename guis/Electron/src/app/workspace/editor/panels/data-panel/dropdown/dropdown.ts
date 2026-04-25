import { Component, input, signal, model, effect, SkipSelf, Optional } from "@angular/core";
import { Dtype, DtypeCounter } from "../../../../../types";
import { AppData } from "../../../../../app_data";

@Component({
  selector: "dropdown",
  templateUrl: "dropdown.html",
  styleUrls: ["../data-panel.css", "dropdown.css"],
})
export class Dropdown {
  name = input("unnamed");
  icon = input("");
  kind = input.required<Dtype>();
  id = input.required<string>();

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
    const isSelected = this.selected();
    const id = this.id();
    const kind = this.kind();

    this.appData.selection.update(old => {
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
    this.appData.deselect();
    this.selected.set(false);
  })
  manuallySelected = false;
  selected = signal(false);

  constructor(
    @Optional() @SkipSelf() private parent: Dropdown,
    public appData: AppData
  ) { }

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

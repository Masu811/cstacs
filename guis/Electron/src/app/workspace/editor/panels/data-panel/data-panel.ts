import { Component, model, input, signal, computed } from "@angular/core";
import { Panel } from "../panel";
import { Dropdown } from "./dropdown/dropdown";
import { MultiCampaign, Dtype, DtypeCounter, DtypeToggle, DtypeSelection } from "../../../../types";
import { Datatypes } from "./datatypes/datatypes";
import { Leaf } from "./leaf/leaf";

@Component({
  selector: "data-panel",
  templateUrl: "data-panel.html",
  styleUrls: ["../panel.css", "data-panel.css"],
  imports: [Dropdown, Datatypes, Leaf],
})
export class DataPanel extends Panel {
  data = model(Array<MultiCampaign>());

  dtypes = Dtype;
  availDtypes = input.required<DtypeToggle>();

  selection = model.required<DtypeSelection>();
  deselect = input.required<boolean>();

  openCounter = signal({
    [Dtype.MULT]: 0,
    [Dtype.MC]: 0,
    [Dtype.M]: 0,
  } as DtypeCounter);
  multOpen = computed(() => this.openCounter()[Dtype.MULT] > 0);
  mcOpen = computed(() => this.openCounter()[Dtype.MC] > 0);
  mOpen = computed(() => this.openCounter()[Dtype.M] > 0);
  multToggle = signal([false]);
  mcToggle = signal([false]);
  mToggle = signal([false]);

  handleDtypeToggle(type: Dtype) {
    switch (type) {
      case Dtype.MULT:
        this.multToggle.set([!this.multOpen()]);
        break;
      case Dtype.MC:
        this.mcToggle.set([!this.mcOpen()]);
        break;
      case Dtype.M:
        this.mToggle.set([!this.mOpen()]);
        break;
    }
  }

  select(event: Event, idcs: Array<number | string>) {
    event.stopPropagation();
  }
}

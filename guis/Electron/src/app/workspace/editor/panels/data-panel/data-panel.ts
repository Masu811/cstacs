import { Component, model, input, signal } from "@angular/core";
import { Panel } from "../panel";
import { Dropdown } from "./dropdown/dropdown";
import { MultiCampaign, Selection, Dtype, DtypeCounter } from "../../../../types";
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
  details = model({});

  dtypes = Dtype;
  availDtypes = input.required<Selection>();
  openCounter = signal({
    [Dtype.MULT]: 0,
    [Dtype.MC]: 0,
    [Dtype.M]: 0,
  } as DtypeCounter);
  dtypeDisplayLevel = signal(0);

  async fetchMetadata(idcs: Array<number | string>) {
    const response = await fetch(`http://127.0.0.1:8000/getMetadata/${idcs.join("-")}`);
    if (!response.ok) {
      console.error(`Failed to fetch metadata for indices ${idcs}`);
      return;
    }
    const data = await response.json();
    this.details.set(data);
  }
}

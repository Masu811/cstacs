import { Component, model, ModelSignal } from "@angular/core";
import { Panel } from "../panel";
import { Dropdown } from "./dropdown/dropdown";
import { MultiCampaign } from "../../../../types";

@Component({
  selector: "data-panel",
  templateUrl: "data-panel.html",
  styleUrls: ["../panel.css", "data-panel.css"],
  imports: [Dropdown],
})
export class DataPanel extends Panel {
  data: ModelSignal<Array<MultiCampaign>> = model(Array<MultiCampaign>());
  details = model({});

  async fetchMetadata(idcs: Array<number | string>) {
    idcs[0] = this.data()[idcs[0] as number]?.id;
    const response = await fetch(`http://127.0.0.1:8000/getMetadata/${idcs.join("/")}`);
    if (!response.ok) {
      console.error(`Failed to fetch metadata for indices ${idcs}`);
      return;
    }
    const data = await response.json();
    this.details.set(data);
  }
}

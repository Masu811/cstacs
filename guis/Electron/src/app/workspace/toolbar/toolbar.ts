import { Component, model } from "@angular/core";
import { MultiCampaign } from "../../types";

@Component({
  selector: "toolbar",
  templateUrl: "toolbar.html",
  styleUrl: "toolbar.css",
})
export class Toolbar {
  data = model(Array<MultiCampaign>());
  projectLoaded = model(false);

  async importData() {
    const path = await (window as any).api.showOpenDialog();

    if (path == null) {
      return;
    }

    const response = await fetch(`http://127.0.0.1:8000/import_data?path=${path}`);
    const newData = await response.json() as MultiCampaign;

    this.data.update(oldData => [...oldData, newData]);
    this.projectLoaded.set(true);
  }
}

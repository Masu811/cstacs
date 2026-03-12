import { Component, model, ModelSignal } from "@angular/core";
import { MultiCampaign } from "../../types";

@Component({
  selector: "topbar",
  templateUrl: "topbar.html",
  styleUrl: "topbar.css",
})
export class TopBar {
  data: ModelSignal<Array<MultiCampaign>> = model(Array<MultiCampaign>());
  projectLoaded = model(false);

  async importData() {
    const path = await (window as any).api.showOpenDialog();

    if (path == null) {
      return;
    }

    const response = await fetch(`http://127.0.0.1:8000/import_data?path=${path}`);
    const newData = await response.json() as MultiCampaign;
    this.data.update(data => {
      data.push(newData);
      return data;
    });
    this.projectLoaded.set(true);
  }

  async saveProject() {
    const files = await (window as any).api.showSaveDialog();
  }
}

import { Component } from "@angular/core";
import { Router } from "@angular/router";
import { Dtype, MultiCampaign, DtypeSelection, parseSelection } from "../../types";
import { AppData } from "../../services/app_data";

@Component({
  selector: "toolbar",
  templateUrl: "toolbar.html",
  styleUrl: "toolbar.css",
})
export class Toolbar {
  constructor(private router: Router, public appData: AppData) { }

  async importData() {
    const path = await (window as any).api.showOpenDialog();

    if (path == null) {
      return;
    }

    const response = await fetch(`http://127.0.0.1:8000/import_data?path=${path}`);
    const newData = await response.json() as Array<MultiCampaign>;

    this.appData.data.set(newData);
    this.appData.projectLoaded.set(true);
  }

  async deleteData() {
    const payload = parseSelection(this.appData.selection());

    const response = await fetch("http://127.0.0.1:8000/delete_data", {
      method: "POST",
      headers: {
        "Content-Type": "application/json"
      },
      body: JSON.stringify(payload),
    });

    const newData = await response.json() as Array<MultiCampaign>;

    this.appData.deselect.update(val => !val);
    this.appData.selection.set({
      [Dtype.MULT]: new Set<string>([]),
      [Dtype.MC]: new Set<string>([]),
      [Dtype.M]: new Set<string>([]),
      [Dtype.S]: new Set<string>([]),
      [Dtype.C]: new Set<string>([]),
    } as DtypeSelection);
    this.appData.data.set(newData);
  }

  async fetchMetadata(kind: Dtype) {
    const idcs = parseSelection(this.appData.selection())[kind][0];

    const response = await fetch(`http://127.0.0.1:8000/getMetadata/${idcs}`);
    if (!response.ok) {
      console.error(`Failed to fetch metadata for indices ${idcs}`);
      return;
    }
    const data = await response.json();

    this.appData.metadata.set(data);
    this.router.navigate(["/metadata"]);
  }

  analyzeSingles() {
    this.appData.dialogType.set("SingleAnalyze");
    this.appData.dialogOpen.set(true);
  }
}

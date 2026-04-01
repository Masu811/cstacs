import { Component, model, input, computed } from "@angular/core";
import { Router } from "@angular/router";
import { Dtype, MultiCampaign, DtypeToggle, DtypeSelection, ParsedSelection, parseSelection } from "../../types";
import { MetadataService } from "../../services/metadata";

@Component({
  selector: "toolbar",
  templateUrl: "toolbar.html",
  styleUrl: "toolbar.css",
})
export class Toolbar {
  data = model(Array<MultiCampaign>());
  projectLoaded = model(false);

  availDtypes = input.required<DtypeToggle>();
  dtypes = Dtype;

  selection = model.required<DtypeSelection>();
  deselect = model.required<boolean>();

  anySelected = computed(() => {
    return Object.values(this.selection()).some(val => val);
  });

  dialogType = model.required<string>();
  dialogOpen = model.required<boolean>();

  constructor(private router: Router, private metadata: MetadataService) { }

  async importData() {
    const path = await (window as any).api.showOpenDialog();

    if (path == null) {
      return;
    }

    const response = await fetch(`http://127.0.0.1:8000/import_data?path=${path}`);
    const newData = await response.json() as Array<MultiCampaign>;

    this.data.set(newData);
    this.projectLoaded.set(true);
  }

  async deleteData() {
    const payload = parseSelection(this.selection());

    const response = await fetch("http://127.0.0.1:8000/delete_data", {
      method: "POST",
      headers: {
        "Content-Type": "application/json"
      },
      body: JSON.stringify(payload),
    });

    const newData = await response.json() as Array<MultiCampaign>;

    this.deselect.update(val => !val);
    this.selection.set({
      [Dtype.MULT]: new Set<string>([]),
      [Dtype.MC]: new Set<string>([]),
      [Dtype.M]: new Set<string>([]),
      [Dtype.S]: new Set<string>([]),
      [Dtype.C]: new Set<string>([]),
    } as DtypeSelection);
    this.data.set(newData);
  }

  async fetchMetadata(kind: Dtype) {
    const idcs = parseSelection(this.selection())[kind][0];

    const response = await fetch(`http://127.0.0.1:8000/getMetadata/${idcs}`);
    if (!response.ok) {
      console.error(`Failed to fetch metadata for indices ${idcs}`);
      return;
    }
    const data = await response.json();

    this.metadata.metadata.set(data);
    this.router.navigate(["/metadata"]);
  }

  analyzeSingles() {
    this.dialogType.set("SingleAnalyze");
    this.dialogOpen.set(true);
  }
}

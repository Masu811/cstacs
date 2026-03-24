import { Component, model, input, computed } from "@angular/core";
import { Router } from "@angular/router";
import { Dtype, MultiCampaign, DtypeToggle, Metadata, DtypeSelection } from "../../types";
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

  selection = input.required<DtypeSelection>();

  anySelected = computed(() => {
    return Object.values(this.selection()).some(val => val);
  });

  constructor(private router: Router, private metadata: MetadataService) { }

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

  async fetchMetadata(kind: Dtype) {
    const id = Array.from(this.selection()[kind])[0];
    const idcs = id.replaceAll(",", "-");

    const response = await fetch(`http://127.0.0.1:8000/getMetadata/${idcs}`);
    if (!response.ok) {
      console.error(`Failed to fetch metadata for indices ${idcs}`);
      return;
    }
    const data = await response.json();

    this.metadata.metadata.set(data);
    this.router.navigate(["/metadata"]);
  }
}

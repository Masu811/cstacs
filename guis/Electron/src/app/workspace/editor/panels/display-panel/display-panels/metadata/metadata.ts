import { Component, computed } from "@angular/core";
import { Metadata } from "../../../../../../types";
import { AppData } from "../../../../../../app_data";

@Component({
  selector: "metadata",
  templateUrl: "metadata.html",
  styleUrl: "metadata.css",
})
export class MetadataPanel {
  name = computed(() => this.appData.metadata()?.__name__);

  elements = computed(() => {
    const metadata: Metadata | null = this.appData.metadata();

    if (!metadata) {
      return [];
    }

    return Object.entries(metadata)
      .filter(item => item[0] != "__name__")
      .map(item => `${item[0]}: ${item[1]}`);
  });

  constructor(public appData: AppData) { }
}

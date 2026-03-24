import { Component, computed } from "@angular/core";
import { Metadata } from "../../../../../../types";
import { MetadataService } from "../../../../../../services/metadata";

@Component({
  selector: "metadata",
  templateUrl: "metadata.html",
  styleUrl: "metadata.css",
})
export class MetadataPanel {
  name = computed(() => this.metadata.metadata()?.__name__);

  elements = computed(() => {
    const metadata: Metadata | null = this.metadata.metadata();

    if (!metadata) {
      return [];
    }

    return Object.entries(metadata)
      .filter(item => item[0] != "__name__")
      .map(item => `${item[0]}: ${item[1]}`);
  });

  constructor(private metadata: MetadataService) { }
}

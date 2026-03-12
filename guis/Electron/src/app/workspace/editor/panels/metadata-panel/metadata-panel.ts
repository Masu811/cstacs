import { Component, model, computed, ModelSignal } from "@angular/core";
import { Panel } from "../panel";

type metadata = {
  __name__?: string,
}

@Component({
  selector: "metadata-panel",
  templateUrl: "metadata-panel.html",
  styleUrls: ["../panel.css", "metadata-panel.css"],
})
export class MetadataPanel extends Panel {
  details: ModelSignal<metadata> = model({});
  name = computed(() => this.details().__name__);

  elements() {
    const details: metadata = this.details();
    return Object.entries(details)
      .filter(item => item[0] != "__name__")
      .map(item => `${item[0]}: ${item[1]}`);
  }
}

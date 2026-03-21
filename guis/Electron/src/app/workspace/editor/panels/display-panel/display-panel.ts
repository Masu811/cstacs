import { Component, model, computed, ModelSignal } from "@angular/core";
import { Panel } from "../panel";

type metadata = {
  __name__?: string,
}

@Component({
  selector: "display-panel",
  templateUrl: "display-panel.html",
  styleUrls: ["../panel.css", "display-panel.css"],
})
export class DisplayPanel extends Panel {
  details: ModelSignal<metadata> = model({});
  name = computed(() => this.details().__name__);

  elements() {
    const details: metadata = this.details();
    return Object.entries(details)
      .filter(item => item[0] != "__name__")
      .map(item => `${item[0]}: ${item[1]}`);
  }

  async plot() {
    const Plotly = await this.getPlotly();

    var trace1 = {
      x: [1, 2, 3, 4],
      y: [10, 15, 13, 17],
      type: 'scatter'
    };

    var trace2 = {
      x: [1, 2, 3, 4],
      y: [16, 5, 11, 9],
      type: 'scatter'
    };

    var data = [trace1, trace2];

    //@ts-ignore
    Plotly.newPlot('canvas', data);
  }

  async getPlotly() {
    if (typeof window === "undefined") {
      return null;
    }

    const module = await import("plotly.js-dist-min");
    return module.default;
  }
}

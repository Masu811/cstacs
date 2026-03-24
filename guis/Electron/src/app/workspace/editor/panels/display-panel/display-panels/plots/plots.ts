import { Component } from "@angular/core";

@Component({
  selector: "plots",
  templateUrl: "plots.html",
  styleUrl: "plots.css",
})
export class PlotPanel {
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

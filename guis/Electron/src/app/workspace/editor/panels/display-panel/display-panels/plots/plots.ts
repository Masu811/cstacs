import { Component, effect, ElementRef, signal, ViewChild } from "@angular/core";
import { AppData } from "../../../../../../app_data";
import { PlotTrace } from "../../../../../../types";

@Component({
  selector: "plots",
  templateUrl: "plots.html",
  styleUrl: "plots.css",
})
export class PlotPanel {
  constructor(public appData: AppData) { }

  private plotly: any | null = null;
  @ViewChild('canvas') canvas!: ElementRef;

  plotEffect = effect(() => {
    this.plot(this.appData.plotData());
  });

  async plot(data: Array<PlotTrace>) {
    if (this.plotly == null) {
      await this.getPlotly();
    }
    if (this.plotly == null) return;

    const canvas = this.canvas.nativeElement;

    const layout = {
      autosize: true,
    };

    const config = {
      responsive: true,
      displayModeBar: true,
      modeBarButtonsToAdd: [
        {
          name: 'Toggle log Y',
          icon: this.plotly.Icons.autoscale,
          click: (gd: any) => {
            const currentType = gd.layout?.yaxis?.type ?? 'linear';

            const newType = currentType === 'log' ? 'linear' : 'log';

            this.plotly.relayout(gd, {
              'yaxis.type': newType
            });
          }
        }
      ]
    };

    if (!(canvas as any)._fullLayout) {
      await this.plotly.newPlot(canvas, data, layout, config);
    } else {
      await this.plotly.react(canvas, data, layout, config);
    }
  }

  async getPlotly() {
    if (typeof window === "undefined") {
      console.error("Failed to load Plotly module");
      return null;
    }

    if (!this.plotly) {
      const module = await import("plotly.js-dist-min");
      this.plotly = module.default;
    }

    return this.plotly;
  }
}

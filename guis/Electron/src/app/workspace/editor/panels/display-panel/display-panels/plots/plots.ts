import { Component, effect, ElementRef, ViewChild } from "@angular/core";
import { AppData } from "../../../../../../app_data";
import { PlotConfig, PlotAxes, PlotTrace, PlotLayout } from "../../../../../../types";

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

    const styles = getComputedStyle(document.documentElement);
    const bgColor = styles.getPropertyValue("--editor-color");
    const fontColor = styles.getPropertyValue("--font-color");
    const hlColor = styles.getPropertyValue("--highlighted-editor-color");

    const n_plots = this.appData.n_plots;

    let n_rows, n_cols;

    if (n_plots < 3) {
      n_rows = 1;
      n_cols = n_plots;
    } else if (n_plots == 3) {
      n_rows = n_plots;
      n_cols = 1;
    } else {
      n_rows = Math.round(Math.ceil(Math.sqrt(n_plots)));
      n_cols = Math.round(Math.ceil(n_plots / n_rows));
    }

    const layout: PlotLayout = {
      autosize: true,
      paper_bgcolor: bgColor,
      plot_bgcolor: bgColor,
      font: {
        color: fontColor,
      },
      grid: {
        rows: n_rows,
        columns: n_cols,
        pattern: "independent",
      }
    };

    const axesLayout = this.appData.plotAxes;

    for (const [key, value] of Object.entries(axesLayout)) {
      //@ts-ignore
      layout[key] = {
        ...value,
        showgrid: true,
        gridcolor: hlColor,
      }
    }

    const config: PlotConfig = {
      responsive: true,
      editable: true,
      scrollZoom: true,
      displayModeBar: true,
      modeBarButtonsToAdd: [
        {
          name: 'Toggle log Y',
          title: 'Toggle log Y',
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

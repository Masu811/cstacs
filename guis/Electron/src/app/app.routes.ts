import { Routes } from '@angular/router';
import { PlotPanel } from './workspace/editor/panels/display-panel/display-panels/plots/plots';
import { MetadataPanel } from './workspace/editor/panels/display-panel/display-panels/metadata/metadata';
import { ConsolePanel } from './workspace/editor/panels/display-panel/display-panels/console/console';

export const routes: Routes = [
  {
    path: "",
    component: undefined,
  },
  {
    path: "plots",
    component: PlotPanel,
  },
  {
    path: "metadata",
    component: MetadataPanel,
  },
  {
    path: "console",
    component: ConsolePanel,
  }
];

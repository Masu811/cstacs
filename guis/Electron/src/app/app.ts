import { Component, computed, signal, WritableSignal } from '@angular/core';
import { TopBar } from './workspace/topbar/topbar';
import { Toolbar } from './workspace/toolbar/toolbar';
import { Editor } from './workspace/editor/editor';
import { BottomBar } from './workspace/bottombar/bottombar';
import { MultiCampaign, DtypeToggle, Dtype, DtypeCounter } from './types';

@Component({
  selector: 'app-root',
  templateUrl: './app.html',
  styleUrl: './app.css',
  imports: [TopBar, Toolbar, Editor, BottomBar],
})
export class App {
  protected readonly title = signal('stacs');

  data = signal(Array<MultiCampaign>());
  projectLoaded = signal(false);
  availDtypes = computed(() => {
    const data = this.data();

    let mult = data.length > 0;
    let mc = false;
    let m = false;
    let s = false;
    let c = false;

    outer: for (const multCamp of data) {
      if (multCamp.campaigns.length == 0) continue;

      mc = true;

      for (const measCamp of multCamp.campaigns) {
        if (measCamp.measurements.length == 0) continue;

        m = true;

        for (const doppler of measCamp.measurements) {
          if (doppler.singles.length > 0) s = true;
          if (doppler.coinc.length > 0) c = true;

          if (mult && mc && m && s && c) {
            break outer;
          }
        }
      }
    }

    return {
      [Dtype.MULT]: mult,
      [Dtype.MC]: mc,
      [Dtype.M]: m,
      [Dtype.S]: s,
      [Dtype.C]: c,
    } as DtypeToggle;
  });

  selection = signal({
    [Dtype.MULT]: 0,
    [Dtype.MC]: 0,
    [Dtype.M]: 0,
    [Dtype.S]: 0,
    [Dtype.C]: 0,
  } as DtypeCounter);
}

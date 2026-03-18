import { Component, computed, signal, WritableSignal } from '@angular/core';
import { TopBar } from './workspace/topbar/topbar';
import { Toolbar } from './workspace/toolbar/toolbar';
import { Editor } from './workspace/editor/editor';
import { BottomBar } from './workspace/bottombar/bottombar';
import { MultiCampaign, Selection } from './types';

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
  datatypes = computed(() => {
    const data = this.data();

    let campaigns = false;
    let doppler = false;
    let single = false;
    let coinc = false;

    outer: for (const mult of data) {
      if (mult.campaigns.length == 0) continue;

      campaigns = true;

      for (const mc of mult.campaigns) {
        if (mc.measurements.length == 0) continue;

        doppler = true;

        for (const m of mc.measurements) {
          if (m.singles.length > 0) single = true;
          if (m.coinc.length > 0) coinc = true;

          if (campaigns && doppler && single && coinc) {
            break outer;
          }
        }
      }
    }

    return { campaigns, doppler, single, coinc } as Selection;
  });
}

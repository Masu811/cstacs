import { Component, signal, model, WritableSignal } from '@angular/core';
import { TopBar } from './workspace/topbar/topbar';
import { Toolbar } from './workspace/toolbar/toolbar';
import { Editor } from './workspace/editor/editor';
import { BottomBar } from './workspace/bottombar/bottombar';
import { MultiCampaign } from './types';

@Component({
  selector: 'app-root',
  templateUrl: './app.html',
  styleUrl: './app.css',
  imports: [TopBar, Toolbar, Editor, BottomBar],
})
export class App {
  protected readonly title = signal('stacs');

  data: WritableSignal<Array<MultiCampaign>> = signal(Array<MultiCampaign>());
  projectLoaded = signal(false);
}

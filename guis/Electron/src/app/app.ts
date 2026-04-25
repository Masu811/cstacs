import { Component, signal } from '@angular/core';
import { TopBar } from './workspace/topbar/topbar';
import { Dialog } from './workspace/dialogs/dialog';
import { Toolbar } from './workspace/toolbar/toolbar';
import { Editor } from './workspace/editor/editor';
import { BottomBar } from './workspace/bottombar/bottombar';
import { AppData } from './app_data';
import { ParsersDialog } from './workspace/dialogs/dialogs/parsers/parsers';

@Component({
  selector: 'app-root',
  templateUrl: './app.html',
  styleUrl: './app.css',
  imports: [TopBar, Dialog, ParsersDialog, Toolbar, Editor, BottomBar],
})
export class App {
  protected readonly title = signal('stacs');

  constructor(public appData: AppData) { }
}

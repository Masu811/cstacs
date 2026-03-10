import { Component, signal } from '@angular/core';
import { MenuBar } from './workspace/menubar/menubar';
import { Editor } from './workspace/editor/editor';

@Component({
  selector: 'app-root',
  templateUrl: './app.html',
  styleUrl: './app.css',
  imports: [MenuBar, Editor],
})
export class App {
  protected readonly title = signal('stacs');
}

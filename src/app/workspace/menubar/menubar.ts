import { Component } from "@angular/core";

@Component({
  selector: "menu-bar",
  templateUrl: "menubar.html",
  styleUrl: "menubar.css",
})
export class MenuBar {
  async importData() {
    const files = await (window as any).dialogs.showOpenDialog();
    console.log(files);
  }

  async saveProject() {
    const files = await (window as any).dialogs.showSaveDialog();
    console.log(files);
  }
}

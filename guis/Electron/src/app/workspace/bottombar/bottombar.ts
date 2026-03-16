import { Component, signal } from "@angular/core";

@Component({
  selector: "bottombar",
  templateUrl: "bottombar.html",
  styleUrl: "bottombar.css",
})
export class BottomBar {
  isServerOk = signal(false);
  cpu = signal("");
  ram = signal("");
  ws: null | WebSocket = null;

  private readonly checkServerIntervalMilliSec = 3000;
  private readonly checkSystemIntervalMilliSec = 3000;

  constructor() {
    this.checkServerHealth();
    setInterval(
      () => this.checkSystemHealth(),
      this.checkSystemIntervalMilliSec
    );
  }

  checkServerHealth() {
    this.ws = new WebSocket("ws://127.0.0.1:8000/health");

    this.ws.onopen = () => {
      this.isServerOk.set(true);
    };

    this.ws.onclose = () => {
      this.isServerOk.set(false);
      setTimeout(
        () => this.checkServerHealth(), this.checkServerIntervalMilliSec
      );
    };

    this.ws.onerror = (err) => {
      console.error(err);
      this.isServerOk.set(false);
      setTimeout(
        () => this.checkServerHealth(), this.checkServerIntervalMilliSec
      );
    };
  }

  async checkSystemHealth() {
    const { cpu, ram } = await (window as any).api.health();

    this.cpu.set(`${cpu}`);
    this.ram.set(`${ram}`);
  }
}

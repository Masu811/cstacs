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

  private readonly checkServerIntervalMilliSec = 5000;
  private readonly checkSystemIntervalMilliSec = 3000;

  constructor() {
    this.checkServerHealth();
    this.checkSystemHealth();
    setInterval(
      () => this.checkServerHealth(),
      this.checkServerIntervalMilliSec
    );
    setInterval(
      () => this.checkSystemHealth(),
      this.checkSystemIntervalMilliSec
    );
  }

  async checkServerHealth() {
    try {
      const response = await fetch("http://127.0.0.1:8000/health", {
        signal: AbortSignal.timeout(this.checkServerIntervalMilliSec)
      });
      if (response.ok) {
        this.isServerOk.set(true);
      } else {
        this.isServerOk.set(false);
      }
    } catch (error) {
      console.log(error);
      this.isServerOk.set(false);
    }
  }

  async checkSystemHealth() {
    const { cpu, ram } = await (window as any).api.health();

    this.cpu.set(`${cpu}`);
    this.ram.set(`${ram}`);
  }
}

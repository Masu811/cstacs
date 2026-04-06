import { Directive, HostListener, ElementRef, AfterViewInit } from '@angular/core';

@Directive({
  selector: '[appResize]'
})
export class ResizeDirective implements AfterViewInit {
  leftElement: HTMLElement | null = null;
  rightElement: HTMLElement | null = null;
  grabbing = false;

  constructor(private el: ElementRef<HTMLElement>) {}

  ngAfterViewInit() {
    this.leftElement = this.el.nativeElement.previousElementSibling as HTMLElement;
    this.rightElement = this.el.nativeElement.nextElementSibling as HTMLElement;
  }

  @HostListener('mousedown', ['$event'])
  onMouseDown(event: MouseEvent) {
    event.preventDefault();
    this.grabbing = true;
    document.body.style.cursor = 'col-resize';
    document.body.style.userSelect = 'none';
  }

  @HostListener('window:mouseup')
  onMouseUp() {
    this.grabbing = false;
    document.body.style.cursor = '';
    document.body.style.userSelect = '';
    window.dispatchEvent(new Event('resize'));
  }

  @HostListener('window:mousemove', ['$event'])
  onMouseMove(event: MouseEvent) {
    if (!this.grabbing || !this.leftElement || !this.rightElement) return;

    const container = this.el.nativeElement.parentElement!;
    const rect = container.getBoundingClientRect();

    const min = 100;
    const max = rect.width - 100;

    let newLeftWidth = event.clientX - rect.left;
    newLeftWidth = Math.max(min, Math.min(max, newLeftWidth));

    this.leftElement.style.width = `${newLeftWidth}px`;
    this.rightElement.style.width = `${rect.width - newLeftWidth}px`;
  }
}

import { Component, model, ModelSignal, signal, AfterViewInit, inject, EnvironmentInjector, createComponent, Type, inputBinding, Binding, twoWayBinding, ApplicationRef } from "@angular/core";

import { ComponentContainer, GoldenLayout, LayoutConfig } from 'golden-layout'

import { MultiCampaign } from "../../types";
import { Welcome } from "./panels/welcome-panel/welcome";
import { DataPanel } from "./panels/data-panel/data-panel";
import { MetadataPanel } from "./panels/metadata-panel/metadata-panel";
import { VizPanel } from "./panels/viz-panel/viz-panel";

@Component({
  selector: "editor",
  templateUrl: "editor.html",
  styleUrl: "editor.css",
})
export class Editor implements AfterViewInit {
  projectLoaded = model(false);
  data: ModelSignal<Array<MultiCampaign>> = model(Array<MultiCampaign>());
  details = signal({});

  private envInjector = inject(EnvironmentInjector);

  constructor(private applicationRef: ApplicationRef) { }

  ngAfterViewInit() {
    const editor = document.getElementById("editor");

    if (editor == null) {
      console.error("Could not find editor element");
      return;
    }

    const myLayout = new GoldenLayout(editor);

    const registerPanel = (
      name: string,
      comp: Type<any>,
      bindings: Array<Binding>,
    ) => {
      myLayout.registerComponentFactoryFunction(
        name, (container: ComponentContainer) => {
          const elem = container.element;

          const ref = createComponent(comp, {
            environmentInjector: this.envInjector,
            hostElement: elem,
            bindings: bindings
          });

          this.applicationRef.attachView(ref.hostView);
          ref.changeDetectorRef.detectChanges();

          container.on('destroy', () => ref.destroy());
        }
      );
    };

    registerPanel(
      'WelcomeComponent', Welcome, [
        inputBinding('projectLoaded', this.projectLoaded)
      ]
    );
    registerPanel(
      'DataPanelComponent', DataPanel, [
        twoWayBinding('data', this.data),
        twoWayBinding('details', this.details),
      ]
    );
    registerPanel(
      'MetadataPanelComponent', MetadataPanel, [
        twoWayBinding('details', this.details),
      ]
    );
    registerPanel(
      'VizPanelComponent', VizPanel, [
        inputBinding('data', this.data),
      ]
    );

    const config: LayoutConfig = {
      root: {
        type: 'row',
        content: [{
          type: 'column',
          content: [{
            type: 'component',
            componentType: 'DataPanelComponent',
            title: 'Data'
          }, {
            type: 'component',
            componentType: 'MetadataPanelComponent',
            title: 'Inspect'
          }]
        }, {
          type: 'component',
          componentType: 'VizPanelComponent',
          title: 'Plots'
        }]
      }
    };

    myLayout.loadLayout(config);
  }
}

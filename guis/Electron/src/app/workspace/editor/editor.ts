import {
  Component, model, ModelSignal, signal, AfterViewInit, inject,
  EnvironmentInjector, createComponent, Type, inputBinding, Binding,
  twoWayBinding, ApplicationRef, input
} from "@angular/core";

import { ComponentContainer, GoldenLayout, LayoutConfig } from 'golden-layout'

import { MultiCampaign, Selection } from "../../types";
import { Welcome } from "./panels/welcome-panel/welcome";
import { DataPanel } from "./panels/data-panel/data-panel";
import { MetadataPanel } from "./panels/metadata-panel/metadata-panel";
import { VizPanel } from "./panels/viz-panel/viz-panel";
import { EmptyPanel } from "./panels/empty-panel/empty-panel";

@Component({
  selector: "editor",
  templateUrl: "editor.html",
  styleUrl: "editor.css",
})
export class Editor implements AfterViewInit {
  projectLoaded = model(false);
  data = model(Array<MultiCampaign>());

  availDtypes = input.required<Selection>();

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
      'EmptyPanelComponent', EmptyPanel, []
    );
    registerPanel(
      'DataPanelComponent', DataPanel, [
        inputBinding('availDtypes', this.availDtypes),
        twoWayBinding('data', this.data),
      ]
    );
    // registerPanel(
    //   'MetadataPanelComponent', MetadataPanel, [
    //   ]
    // );
    registerPanel(
      'VizPanelComponent', VizPanel, [
        inputBinding('data', this.data),
      ]
    );

    const config: LayoutConfig = {
      root: {
        type: 'row',
        content: [{
          type: 'component',
          componentType: 'DataPanelComponent',
          title: 'Data',
          size: "20%",
        }, {
          type: 'stack',
          content: [{
            type: 'component',
            componentType: 'VizPanelComponent',
            title: 'Plots',
          }],
        }],
      },
      settings:{
          showPopoutIcon: false,
      },
      dimensions: {
        borderWidth: 1,
        headerHeight: 25,
      }
    };

    myLayout.loadLayout(config);
  }
}

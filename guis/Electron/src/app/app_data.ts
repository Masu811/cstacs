import { Injectable, signal, computed } from "@angular/core";
import { MultiCampaign, Dtype, DtypeToggle, DtypeSelection, PlotTrace, Metadata } from "./types";
import { Parser } from "./workspace/dialogs/dialogs/parsers/parser-arg-types";

@Injectable({ providedIn: "root" })
export class AppData {
  data = signal(Array<MultiCampaign>());
  plotData = signal(Array<PlotTrace>());
  metadata = signal<Metadata | null>(null);
  tableData = signal<any>([]);

  projectLoaded = signal(false);
  dtypes = Dtype;
  availDtypes = computed(() => {
    const data = this.data();

    let mult = data.length > 0;
    let mc = false;
    let m = false;
    let s = false;
    let c = false;

    outer: for (const multCamp of data) {
      if (multCamp.campaigns.length == 0) continue;

      mc = true;

      for (const measCamp of multCamp.campaigns) {
        if (measCamp.measurements.length == 0) continue;

        m = true;

        for (const doppler of measCamp.measurements) {
          if (doppler.singles.length > 0) s = true;
          if (doppler.coinc.length > 0) c = true;

          if (mult && mc && m && s && c) {
            break outer;
          }
        }
      }
    }

    return {
      [Dtype.MULT]: mult,
      [Dtype.MC]: mc,
      [Dtype.M]: m,
      [Dtype.S]: s,
      [Dtype.C]: c,
    } as DtypeToggle;
  });

  selection = signal({
    [Dtype.MULT]: new Set<string>([]),
    [Dtype.MC]: new Set<string>([]),
    [Dtype.M]: new Set<string>([]),
    [Dtype.S]: new Set<string>([]),
    [Dtype.C]: new Set<string>([]),
  } as DtypeSelection);
  deselect = signal(false);
  anySelected = computed(() => {
    return Object.values(this.selection()).some(val => val);
  });

  dialogType = signal("none");
  dialogOpen = signal(false);
  parsersOpen = signal(false);
  parserSelected = signal<Parser>({
    name: "",
    args: {},
    repr: "",
  });
  parserIndex: number = 0;
}

import { Component, input } from "@angular/core";
import { Dtype } from "../../../../../types";

@Component({
  selector: "leaf",
  templateUrl: "leaf.html",
  styleUrls: ["../data-panel.css", "leaf.css"],
})
export class Leaf {
  name = input("unnamed");
  icon = input("");
  kind = input.required<Dtype>();
  dtypes = Dtype;
}

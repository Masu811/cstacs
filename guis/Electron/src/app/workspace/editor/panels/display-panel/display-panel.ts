import { Component, input, computed} from "@angular/core";
import { RouterOutlet } from "@angular/router";
import { Panel } from "../panel";
import { Metadata } from "../../../../types";

@Component({
  selector: "display-panel",
  templateUrl: "display-panel.html",
  styleUrls: ["../panel.css", "display-panel.css"],
  imports: [RouterOutlet],
})
export class DisplayPanel extends Panel { }

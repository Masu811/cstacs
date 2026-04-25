import { Component, computed } from "@angular/core";
import { AppData } from "../../../../../../app_data";

@Component({
  selector: "data-table",
  templateUrl: "table.html",
  styleUrl: "table.css",
})
export class Table {
  constructor(public appData: AppData) { }

  rows(table: any) {
    const columns = Object.values(table);
    if (!columns.length) return [];
    //@ts-ignore
    return columns[0].map((_: any, i: number) => columns.map(column => column[i]));
  }

  headers(table: any) {
    return Object.keys(table);
  }
}

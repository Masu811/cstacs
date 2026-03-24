import { Injectable, signal } from "@angular/core";
import { Metadata } from "../types";

@Injectable({ providedIn: "root" })
export class MetadataService {
  metadata = signal<Metadata | null>(null);
}

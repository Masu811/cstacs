export type DopplerMeasurement = {
  "id": string,
  "name": string | null,
  "singles": Array<string>,
  "coinc": Array<string>,
}

export type MeasurementCampaign = {
  "id": string,
  "name": string | null,
  "measurements": Array<DopplerMeasurement>,
}

export type MultiCampaign = {
  "id": string,
  "name": string | null,
  "campaigns": Array<MeasurementCampaign>,
}

export enum Dtype {
  MULT = "MULT",
  MC = "MC",
  M = "M",
  S = "S",
  C = "C"
}

export type DtypeToggle = {
  [Dtype.MULT]: boolean,
  [Dtype.MC]: boolean,
  [Dtype.M]: boolean,
  [Dtype.S]: boolean,
  [Dtype.C]: boolean,
}

export type DtypeCounter = {
  [Dtype.MULT]: number,
  [Dtype.MC]: number,
  [Dtype.M]: number,
  [Dtype.S]: number,
  [Dtype.C]: number,
}

export type DtypeSelection = {
  [Dtype.MULT]: Set<string>,
  [Dtype.MC]: Set<string>,
  [Dtype.M]: Set<string>,
  [Dtype.S]: Set<string>,
  [Dtype.C]: Set<string>,
}

export type Metadata = {
  __name__: string,
}

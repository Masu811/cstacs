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
  MULT,
  MC,
  M,
  S,
  C,
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

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

export type Selection = {
  campaigns: boolean,
  doppler: boolean,
  single: boolean,
  coinc: boolean,
}

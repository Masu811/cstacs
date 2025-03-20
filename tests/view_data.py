from stacs import DopplerMeasurement

m = DopplerMeasurement("./testdata/CDB_spectra_WRe-Alloys_0000.n42", autocompute_singles=False)

print(m["OAB"].ecal)

m.show_singles()

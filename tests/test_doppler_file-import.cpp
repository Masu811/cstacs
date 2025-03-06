#include <stacs/stacs.hpp>
#include <string>
#include <iostream>

int main() {
    std::string filename("./testdata/CDB_spectra_WRe-Alloys_0000.n42");

    DopplerMeasurement m = import_n42(filename);

    std::cout << m.shape() << "\n";

    return 0;
}

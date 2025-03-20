#include <string>
#include <iostream>

#include <stacs/stacs.hpp>

int main() {
    std::string filename("./testdata/CDB_spectra_WRe-Alloys_0000.n42");

    DopplerMeasurement m(filename);

    std::cout << m.shape() << "\n";

    return 0;
}

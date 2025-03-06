#ifndef IMPORTER_HPP
#define IMPORTER_HPP

#include <string>
#include <vector>

#include "measurement.hpp"

std::vector<std::vector<unsigned int>> import_png(const std::string& filename);

DopplerMeasurement import_n42(
    const std::string& filename,
    const bool autocompute_singles = true,
    const bool autocompute_coinc = true,
    const bool skip_coinc = false);

#endif
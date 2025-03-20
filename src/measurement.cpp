#include "measurement.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <utility>

#include "importer.hpp"
#include "single.hpp"
#include "coinc.hpp"


std::ostream& operator<<(std::ostream& os, const Shape& shape) {
    os << "(s=" << shape.s << ", c=" << shape.c << ")";
    return os;
}


DopplerMeasurement::DopplerMeasurement() = default;


DopplerMeasurement::DopplerMeasurement(const DopplerMeasurement& other)
    : autocompute_singles(other.autocompute_singles),
      autocompute_coinc(other.autocompute_coinc),
      show(other.show),
      verbose(other.verbose),
      name(other.name),
      path(other.path),
      directory(other.directory),
      filename(other.filename),
      filetype(other.filetype),
      singles(other.singles),
      coinc(other.coinc),
      metadata(other.metadata)
{}


DopplerMeasurement::DopplerMeasurement(DopplerMeasurement&& other)
    : autocompute_singles(std::move(other.autocompute_singles)),
      autocompute_coinc(std::move(other.autocompute_coinc)),
      show(std::move(other.show)),
      verbose(std::move(other.verbose)),
      name(std::move(other.name)),
      path(std::move(other.path)),
      directory(std::move(other.directory)),
      filename(std::move(other.filename)),
      filetype(std::move(other.filetype)),
      singles(std::move(other.singles)),
      coinc(std::move(other.coinc)),
      metadata(std::move(other.metadata))
{}


DopplerMeasurement::DopplerMeasurement(const std::string& path)
    : DopplerMeasurement(import_n42(path))
{}


DopplerMeasurement& DopplerMeasurement::operator<<(SingleSpectrum *s) {
    this->singles.push_back(s);
    return *this;
}


DopplerMeasurement& DopplerMeasurement::operator<<(CoincidenceSpectrum *c) {
    this->coinc.push_back(c);
    return *this;
}


Shape DopplerMeasurement::shape() {
    Shape shape;

    shape.s = singles.size();
    shape.c = coinc.size();

    return shape;
}


MeasurementCampaign::MeasurementCampaign() = default;


MeasurementCampaign::MeasurementCampaign(
    std::string path,
    std::vector<DopplerMeasurement*>&& measurements,
    std::string name,
    bool cache,
    std::string cache_path,
    bool autocompute_singles,
    bool autocompute_coinc,
    bool skip_coinc,
    bool show,
    bool verbose)
    : path(path),
      measurements(measurements),
      name(name),
      cache_path(cache_path),
      autocompute_singles(autocompute_singles),
      autocompute_coinc(autocompute_coinc),
      show(show),
      verbose(verbose)
{}
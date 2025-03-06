#ifndef MEASUREMENT_HPP
#define MEASUREMENT_HPP

#include <iostream>
#include <string>
#include <vector>
#include <map>

#include "single.hpp"
#include "coinc.hpp"

class Shape {
public:
    unsigned int s;
    unsigned int c;

    friend std::ostream& operator<<(std::ostream& os, const Shape& shape);
};

class DopplerMeasurement {
private:
    bool autocompute_singles;
    bool autocompute_coinc;
    bool show;
    bool verbose;

    std::string name;
    std::string path;
    std::string directory;
    std::string filename;
    std::string filetype;

public:
    std::vector<SingleSpectrum*> singles;
    std::vector<CoincidenceSpectrum*> coinc;

    std::map<std::string, std::string> metadata;

    // Default constructor
    DopplerMeasurement();

    // Copy constructor
    DopplerMeasurement(const DopplerMeasurement& other);

    // Move constructor
    DopplerMeasurement(DopplerMeasurement&& other);

    // Construct from filename
    DopplerMeasurement(std::string path);

    DopplerMeasurement& operator<<(SingleSpectrum *s);

    DopplerMeasurement& operator<<(CoincidenceSpectrum *c);

    Shape shape();
};

class MeasurementCampaign {
private:
    std::vector<DopplerMeasurement*> measurements;

    bool autocompute_singles;
    bool autocompute_coinc;
    bool show;
    bool verbose;

    std::string name;
    std::string path;
    std::string directory;
    std::string cache_path;

public:
    // Default constructor: empty MeasurementCampaign
    MeasurementCampaign();

    // Full constructor
    MeasurementCampaign(
        std::string path,
        std::vector<DopplerMeasurement*>&& measurements,
        std::string name = "",
        bool cache = false,
        std::string cache_path = "/tmp/stacs_cache.pkl",
        bool autocompute_singles = true,
        bool autocompute_coinc = true,
        bool skip_coinc = false,
        bool show = false,
        bool verbose = false);
};

class MultiCampaign {

};

#endif
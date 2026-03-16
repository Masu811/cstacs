#include "importer.hpp"

#include <iostream>
#include <string>
#include <tuple>
#include <cstring>
#include <sstream>
#include <utility>
#include <filesystem>

// XML Reader Library
#include "rapidxml.hpp"
#include "rapidxml_utils.hpp"  // For rapidxml::file

// PNG Reader Library
#define STB_IMAGE_IMPLEMENTATION
extern "C" {
    #include "../../common_includes/stb_image.h"
}

#include "single.hpp"
#include "coinc.hpp"
#include "measurement.hpp"



std::vector<std::vector<unsigned int>> import_png(const std::string& filename) {
    int width, height, channels;
    unsigned char *img = stbi_load(filename.c_str(), &width, &height, &channels, 3);

    if (img == NULL) {
        std::cerr << "Error: Could not load png file " << filename;
        return {};
    }

    std::vector<std::vector<unsigned int>> hist(height, std::vector<unsigned int>(width));

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int index = (y * width + x) * 3;
            unsigned int value = img[index] | (img[index + 1] << 8) | (img[index + 2] << 16);
            hist[y][x] = value;
        }
    }

    stbi_image_free(img);

    return hist;
}


DopplerMeasurement import_n42(
    const std::string& filename
    // const bool autocompute_singles,
    // const bool autocompute_coinc,
    // const bool skip_coinc
    )
{
    using namespace rapidxml;

    std::filesystem::path directory = std::filesystem::path(filename).parent_path();

    file<> xmlFile(filename.c_str());

    xml_document<> doc;
    doc.parse<0>(xmlFile.data());

    xml_node<> *root = doc.first_node();

    DopplerMeasurement m;

    std::vector<SingleSpectrum*> singles;
    std::vector<CoincidenceSpectrum*> coinc;

    xml_node<> *meas = root->first_node("RadMeasurement");

    unsigned int count_value;
    double ecal_value;

    for (xml_node<> *meta = meas->first_node(); meta; meta = meta->next_sibling()) {
        if (!(meta->name() != "Spectrum")) continue;

        xml_attribute<> *id = meta->first_attribute("id");
        if (!id) continue;

        if (std::strstr(id->value(), "Coinc")) {

            // Get spectrum

            std::filesystem::path png_filename = directory / std::filesystem::path(meta->value());

            std::vector<std::vector<unsigned int>> hist = import_png(png_filename.string());

            if (hist.empty()) continue;

            // Assemble CoincidenceSpectrum

            CoincidenceSpectrum c(std::move(hist));

            m.coinc.push_back(&c);

        } else {

            std::vector<unsigned int> spectrum;
            std::string detname = "";
            std::tuple<double, double> ecal = {NAN, NAN};
            double eres;

            // Get spectrum

            std::istringstream iss(meta->first_node("ChannelData")->value());

            while (iss >> count_value) {
                spectrum.push_back(count_value);
            }

            // Resolve references of detname and ecal

            xml_attribute<> *ecal_id_p = meta->first_attribute("energyCalibrationReference");

            std::string ecal_id;

            if (ecal_id_p) {
                ecal_id = ecal_id_p->value();
            } else {
                ecal_id = "";
            }

            xml_attribute<> *det_id_p = meta->first_attribute("radDetectorInformationReference");

            std::string det_id;

            if (det_id_p) {
                det_id = det_id_p->value();
            } else {
                det_id = "";
            }

            for (xml_node<> *tmp = root->first_node(); tmp; tmp = tmp->next_sibling()) {
                if (strcmp(tmp->name(), "RadDetectorInformation") == 0) {
                    // Get detname
                    xml_attribute<> *id = tmp->first_attribute("id");

                    if (!id) continue;

                    if (strcmp(id->value(), det_id.c_str()) != 0) continue;

                    xml_node<> *detname_p = tmp->first_node("RadDetectorName");

                    if (detname_p) {
                        detname = detname_p->value();
                    } else {
                        detname = "";
                    }
                } else if (strcmp(tmp->name(), "EnergyCalibration") == 0) {
                    // Get ecal
                    xml_attribute<> *id = tmp->first_attribute("id");

                    if (!id) continue;

                    if (strcmp(id->value(), ecal_id.c_str()) != 0) continue;

                    xml_node<> *ecal_p = tmp->first_node("CoefficientValues");

                    if (ecal_p) {
                        std::istringstream iss(ecal_p->value());
                        iss >> std::get<0>(ecal);
                        iss >> std::get<1>(ecal);
                    } else {
                        ecal = {NAN, NAN};
                    }
                }
            }

            // Assemble SingleSpectrum

            SingleSpectrum s(std::move(spectrum), detname, ecal, eres);

            m.singles.push_back(&s);
        }
    }

    return m;
}

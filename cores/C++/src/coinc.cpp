#include "coinc.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <tuple>
#include <numeric>


CoincidenceSpectrum::CoincidenceSpectrum(
    std::vector<std::vector<unsigned int>>&& hist,
    std::tuple<std::tuple<double, double>,
    std::tuple<double, double>> ecal,
    std::tuple<std::tuple<int, int>,
    std::tuple<int, int>> window,
    std::vector<SingleSpectrum*> parents)
    : hist(hist),
      ecal(ecal),
      window(window),
      parents(parents)
{
    counts = std::accumulate(hist.begin(), hist.end(), 0,
        [](int sum, const std::vector<unsigned int>& row){
            return sum + std::accumulate(row.begin(), row.end(), 0);
        });
    dcounts = std::sqrt(counts);
}
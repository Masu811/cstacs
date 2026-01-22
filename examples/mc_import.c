#include <stacs/stacs.h>

int main() {
    MeasurementCampaign *mc = importMeasurementCampaign("data/", 0);

    double v2p_bounds[4] = {400, 500, 506, 516};

    evaluateMeasurementCampaign(
        mc, 1.1, 1.0, 3.0, 0, 60.0, 0, 0, v2p_bounds, 1, 1, 0
    );
    printMeasurementCampaign(mc);
    freeMeasurementCampaign(mc);

    return 0;
}

#include <stacs/stacs.h>

extern int verbose;
extern int debug;

int main()
{
    verbose = 1;
    debug = 0;

    MeasurementCampaign *mc = importMeasurementCampaign("testdata/");
    printMeasurementCampaign(mc);
    evaluateMeasurementCampaign(mc);
    freeMeasurementCampaign(mc);

    return 0;
}

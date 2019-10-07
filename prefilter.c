
/*
 * Functions used to determines latitude, longitude, and arithmetic mean of bin data values
 * for NASA ocean level 3 binned data products
 */

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "prefilter.h"
#include "helpers.h"

struct coordinates {
    double latitude;
    double longitude;
};

/*
 * Function: bin2latlon
 * --------------------
 * Determines latitude and longitude values for provided bin
 * bin: bin id
 * nBinsInRow: array containing the number of bins in each row
 * latrows: array containing the latitude value for each row
 * basebins: array containing the id for the first bin of each row
 * nrows: the number of rows
 * coords: output struct pointer to contain latitude and longitude
 */
void bin2latlon(int bin, const int nBinsInRow[], const double latrows[], int basebins[], int nrows,
                struct coordinates *coords) {
    if (bin < 1) {
        bin = 1;
    }
    int row = findClosestValue(basebins, 0, nrows, bin) - 1;
    double clat = latrows[row];
    double clon;
    clon = 360.0 * (bin - basebins[row] + 0.5) / nBinsInRow[row] - 180.0;
    coords->latitude = clat;
    coords->longitude = clon;
}

void createFullBinArray(int totalBins, int nDataBins, int nrows, const int *dataBins, double fillValue,
                        int *outBins, const double *inData, const double *weights,
                        double *lats, double *lons, int *nBinsInRow, int *basebins,
                        double *outData) {
    double *latrows = (double *) malloc(sizeof(double) * nrows);
    for (int i = 0; i < nrows; ++i) {
        latrows[i] = ((i + 0.5) * 180.0 / nrows) - 90;
        nBinsInRow[i] = (int) (2 * nrows * cos(latrows[i] * M_PI / 180.0) + 0.5);
        if (i == 0) {
            basebins[i] = 1;
        } else {
            basebins[i] = basebins[i - 1] + nBinsInRow[i - 1];
        }
    }
    struct coordinates *coords;
    coords = (struct coordinates *) malloc(sizeof(struct coordinates));
    for (int i = 0; i < totalBins; i++) {
        outBins[i] = i + 1;
        outData[i] = fillValue;
        bin2latlon(outBins[i], nBinsInRow, latrows, basebins, nrows, coords);
        lats[i] = coords->latitude;
        lons[i] = coords->longitude;
    }
    free(coords);
    free(latrows);
    for (int i = 0; i < nDataBins; i++) {
        outData[dataBins[i] - 1] = inData[i] / weights[i];
    }
}

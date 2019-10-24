#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "threshold.h"
#include "filter.h"

/**
 * Determines if a pixel is above the lower threshold or the higher threshold
 * @param value pixel value of interest
 * @param lowThreshold lower threshold to check pixel value against
 * @param highThreshold higher threshold to check pixel value against
 * @param maxValid maximum valid value
 * @return 2 if pixel is above high threshold, 1 if pixel is above low threshold, and 0 if it is below both thresholds
 */
double testThreshold(double value, double lowThreshold, double highThreshold, double maxValid) {
    if (value > highThreshold && value <= maxValid) {
        return 2.;
    } else if (value > lowThreshold && value <= maxValid) {
        return 1.;
    } else {
        return 0.;
    }
}

/**
 * Checks if pixel is either a high pixel or adjacent to a high pixel for hysteresis
 * @param window array of width*width length with pixel of interest at center
 * @param width width of window
 * @return 1 if pixel is connected to or is a high pixel, 0 otherwise
 */
bool isConnected(const double *window, int width) {
    int center = width * (int) ((width - 1) / 2.0) + (int) ((width - 1) / 2.0);
    if (window[center] == 2) {
        return true;
    }
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < width; j++) {
            if (window[i * width + j] == 2 && window[center] >= 1) {
                return true;
            }
        }
    }
    return false;
}

void applyThreshold(int *bins, double *data, double *output, int nbins, int nrows, int *nBinsInRow,
                    int *basebins, double fillValue) {
    int row = 0;
    double *threshold = (double *) malloc(sizeof(double) * nbins);
    double *window = (double *) malloc(sizeof(double) * 9);
    for (int i = 0; i < nbins; i++) {
        if (i == basebins[row] + nBinsInRow[row]) {
            row++;
        }
        if (row < 2 || row > nrows - 3) {
            output[i] = fillValue;
            continue;
        }
        threshold[i] = testThreshold(data[i], 0.4, 0.8, 2.0);
        bool isValid = getWindow(bins[i], row, 3, threshold, nBinsInRow, basebins, window, fillValue, false);
        if (data[i] == fillValue) {
            output[i] = fillValue;
        } else if (isConnected(window, 3) && isValid) {
            output[i] = 1;
        } else if (isValid) {
            output[i] = 0;
        }
    }
    free(window);
    free(threshold);
}

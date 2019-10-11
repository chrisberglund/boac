/**
 * Functions for applying a sobel operator to bins
 */

#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include "sobel.h"
#include "filter.h"

/**
 * Performs a convolution with provided kernel on a given window of same size dimensions.
 * @param window pointer to an array of size * size length containing data values to perform convolution on
 * @param kernel pointer to an array of size * size length containing convolution kernel
 * @param size length of one dimension of the window
 * @return value resulting from convolution
 */
double convolution(const double* window, const int* kernel, int size) {
    double sum = 0;
    for(int i=0; i<size; i++) {
        sum += window[i] * kernel[size-i-1];
    }
    return sum;
}

struct gradient {
    double G;
    double theta;
};

/**
 * Applies a sobel operator to a bin
 * @param window pointer to an array of length 9 containing data values representing a 3x3 window
 * @return structure containing the direction and magnitude of the gradient
 */
struct gradient binSobel(double* window) {
    int GX[9] = {-1, 0, 1, -2, 0, 2, -1, 0, 1};
    int GY[9] = {-1, -2, -1, 0, 0, 0, 1, 2, 1};
    double sobelx = (convolution(window, GX, 9));
    double sobely = convolution(window, GY, 9);
    struct gradient g;
    g.G = sqrt(pow(sobelx, 2) + pow(sobely, 2));
    g.theta = atan2(sobely, sobelx);
    return g;
}

/**
 * Performs nonmax suppression on a a pixel using the manitude and direction of the gradient
 * @param window 3x3 window with pixel of interest in center
 * @param theta direction of the gradient in radians
 * @param width width of the window
 * @return original magnitude value if pixel value is greater than neighboring pixels in the direction, otherwise
 * returns 0.
 */
double nonmaxSupression(double* window, double theta, int width) {
    int center = (int) (width-1)/2 + (width-1)/2;
    double angle = theta * 180. / M_PI;

    double q = 999;
    double r = 999;
    if (angle < 0) {
        angle += 180.;
    }
    if ((angle >= 0. &&  angle < 22.5) || (angle >= 157.5 && angle <= 180)) {
        q = window[center + 1];
        r = window[center - 1];
    }
    else if (angle >= 22.5 && angle < 67.5) {
        q = window[center + width - 1];
        r = window[center - width + 1];
    }
    else if (angle >= 67.5 && angle < 112.5) {
        q = window[center + width];
        r = window[center - width];
    }
    else if (angle >= 112.5 && angle < 157.5) {
        q = window[center - width - 1];
        r = window[center + width + 1];
    }
    if (window[center] >= q && window[center] >= r) {
        return window[center];
    } else {
        return 0.;
    }
}



/**
 * Applies sobel operator to each bin
 * @param bins pointer to an array containing the bin number of each bin in the binning scheme
 * @param data pointer to an array containing the data value for each bin
 * @param dataOut pointer to an empty array of nbins length to write magnitude of sobel operator values to
 * @param thetas pointer to an empty array of nbins length to write direction of magnitude to
 * @param nbins number of bins in the binning scheme
 * @param nrows number of rows in the binning scheme
 * @param nBinsInRow pointer to an array containing the number of bins in each row
 * @param basebins pointer to an array containing the bin number of the first bin in each row
 * @param fillValue value empty bins are filled with
 */
void sobel(int *bins, double *data, double *dataOut, int nbins, int nrows, int *nBinsInRow,
        int *basebins, double fillValue) {
    double *threeWindow = (double *) malloc(sizeof(double) * 9);
    double *magnitudes = (double *) malloc(sizeof(double) * nbins);
    double *thetas = (double *) malloc(sizeof(double) * nbins);
    int row = 0;
    for (int i = 0; i < nbins; i++) {
        double magnitude;
        double theta;
        if (i == basebins[row] + nBinsInRow[row]) {
            row++;
        }
        if (row < 1 || row > nrows - 2) {
            magnitudes[i] = 0;
            thetas[i] = 0;
            continue;
        }
        bool isValid = getWindow(bins[i], row,3, data, nBinsInRow, basebins, threeWindow, fillValue, true);
        if (isValid) {
            struct gradient g = binSobel(threeWindow);
            magnitude = g.G;
            theta = g.theta;
        } else {
            magnitude = 0;
            theta = 0;
        }
        magnitudes[i] = magnitude;
        thetas[i] = theta;
    }
    row = 0;
    for (int i = 0; i < nbins; i++) {
        if (i == basebins[row] + nBinsInRow[row]) {
            row++;
        }
        if (row < 1 || row > nrows - 2) {
            dataOut[i] = 0;
            continue;
        }
        getWindow(bins[i], row,3, magnitudes, nBinsInRow, basebins, threeWindow, fillValue, false);
        dataOut[i] = nonmaxSupression(threeWindow, thetas[i], 3);
    }
    free(magnitudes);
    free(thetas);
    free(threeWindow);
}
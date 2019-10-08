/**
 * Functions for applying a sobel operator to bins
 */

#include <math.h>
#include "sobel.h"
#include "filter.h"
#include "helpers.h"

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

/**
 * Applies a sobel operator to a bin
 * @param window pointer to a 3x3 array containing data values TODO: change implementation to use a 1D array
 * @return magnitude of the result of sobel operation
 */
double binSobel(double** window) {
    double oneDWindow[9];
    flatten2DArray(window, oneDWindow, 3, 3);
    int GX[9] = {-1, 0, 1, -2, 0, 2, -1, 0, 1};
    int GY[9] = {-1, -2, -1, 0, 0, 0, 1, 2, 1};
    double sobelx = (convolution(oneDWindow, GX, 9));
    double sobely = convolution(oneDWindow, GY, 9);
    double res = sqrt(pow(sobelx, 2) + pow(sobely, 2));
    return res;
}

/**
 * Applies sobel operator to each bin
 * @param bins pointer to an array containing the bin number of each bin in the binning scheme
 * @param data pointer to an array containing the data value for each bin
 * @param output pointer to an empty array of nbins length to write magnitude of sobel operator values to
 * @param nbins number of bins in the binning scheme
 * @param nrows number of rows in the binning scheme
 * @param nBinsInRow pointer to an array containing the number of bins in each row
 * @param basebins pointer to an array containing the bin number of the first bin in each row
 * @param fillValue value empty bins are filled with
 */
void sobel(int *bins, double *data, double *output, int nbins, int nrows, int *nBinsInRow,
        int *basebins, double fillValue) {
    double **threeWindow = allocateMatrix(3,3);
    int row = 0;
    for (int i = 0; i < nbins; i++) {
        double value;
        if (bins[i]== basebins[row+1]) {
            row++;
        }
        if (row < 2 || row > nrows - 3) {
            output[i] = fillValue;
            continue;
        }
        getWindow(bins[i], row,3, data, nBinsInRow, basebins, threeWindow, fillValue);
        value = binSobel(threeWindow);
        output[i] = value;
        if (bins[i] == basebins[row+1] - 1) {
            row++;
        }
    }
    freeMatrix(threeWindow, 3);
}
//
// Created by Christopher Berglund on 10/2/19.
//
#include <stdio.h>
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
 * function TODO: Handle fill values
 * @param bins
 * @param data
 * @param output
 * @param nbins
 * @param nrows
 * @param nBinsInRow
 * @param basebins
 * @param fillValue
 */
void sobel(int* bins, double* data, double* output, int nbins, int nrows, int* nBinsInRow, int* basebins, int fillValue) {
    double **threeWindow = allocateMatrix(3,3);
    int row = 0;
    for (int i = 0; i < nbins; i++) {
        double value = data[i];
        if (bins[i] == basebins[row]  || bins[i] == basebins[row+1] - 1 || row == 0 || row == nrows - 1) {
            if (bins[i]== basebins[row+1] - 1) {
                row++;
            }
            output[i] = 0;
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
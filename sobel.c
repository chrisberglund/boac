//
// Created by Christopher Berglund on 10/2/19.
//
#include <stdio.h>
#include <math.h>
#include "sobel.h"
#include "filter.h"
#include "helpers.h"

double convolution(double* window, int* kernel, int size) {
    double sum = 0;
    double test;
    int kernelval;
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

void sobel(int* bins, double* data, double* output, int nbins, int nrows, int* nBinsInRow, int* basebins) {
    double **threeWindow = allocateMatrix(3,3);
    int row = 0;
    int foobar = basebins[row];
    for (int i = 0; i < nbins; i++) {
        foobar = basebins[row];
        int foo = bins[i];
        double value = data[i];
        if (bins[i] == basebins[row]  || bins[i] == basebins[row+1] - 1 || row == 0 || row == nrows - 1) {
            if (bins[i]== basebins[row+1] - 1) {
                row++;
            }
            output[i] = 0;
            continue;
        }
        getWindow(bins[i], row,3, data, nBinsInRow, basebins, threeWindow);
        value = binSobel(threeWindow);
        output[i] = value;
        if (bins[i] == basebins[row+1] - 1) {
            row++;
        }
    }
    freeMatrix(threeWindow, 3);
}
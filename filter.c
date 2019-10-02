#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "filter.h"
#include "helpers.h"


#define TRUE 1
#define FALSE 0

/*
 * Function: isExtrema
 * -------------------
 * Checks if element at given position in an array is either the maximum or minimum value
 * idx: index value of the element check
 * arr: array to check for max or min values
 * length: length of the array
 * return: 1 if element is max or min, 0 if not
 */
int isExtrema(int idx, const double* arr, int length) {
    int isMax = FALSE;
    int isMin = FALSE;
    int isEqual = FALSE;
    double x = arr[idx];

    for (int i = 0; i < length; i++) {
        if (i == idx) {
            continue;
        }
        if (x > arr[i]) {
            isMax = TRUE;
        } else if (x < arr[i]) {
            isMin = TRUE;
        } else if (x == arr[i]) {
            isEqual = TRUE;
        }
    }
    return (isMax != isMin) && (!isEqual);
}

/*
 * Function: isWindowExtrema
 * -------------------------
 * Determines if the center of a square 2-D window is a maximum or minimum along NW-SE, NE-SW, N-S, and E-W axes
 * :width width of the 2-D array
 * :window pointer to 2-D array containing data values
 * return: 1 is center is an extrema and 0 if it is not
 */
int isWindowExtrema(int width, double** window) {
    double *slice = (double *)malloc(sizeof(double)*width);
    int extrema = FALSE;
    int center = (int)(width - 1) / 2;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < width; j++) {
            switch(i) {
                //Northwest to Southeast slice
                case 0:
                    slice[j] = window[j][j];
                    break;

                    //North to South Slice
                case 1:
                    slice[j] = window[j][center];
                    break;

                    //Northeast to Southwest Slice
                case 2:
                    slice[j] = window[j][width-j-1];
                    break;

                    //West to East Slice
                case 3:
                    slice[j] = window[center][j];
                    break;
                default:
                    break;
            }
        }
        if (i == 0)
            extrema = isExtrema(center, slice, width);
        extrema = extrema && isExtrema(center, slice, width);
    }
    free(slice);
    return extrema;
}

/*
 * Function: getNeighborBin
 * ------------------------
 * Finds the bin number north or south of given bin by given distance. This function determines
 * the neighboring bin number by using the ratio between the number of bins in a row difference between the bin numbers
 * of the first bin in a row and the bin of interest. Rounding to the nearest bin number is done.
 * bin: bin number of the bin of interest
 * row: row number of the bin
 * distance: number of rows away from the bin of interest to look for neighbor with positive values for north and
 * negative numbers for south
 * nBinsInRow: pointer to array containing the number of bins in each row
 * basebins: pointer to array containing the bin number of the first bin of each row
 */
int getNeighborBin(int bin, int row, int distance,  const int* nBinsInRow, const int* basebins) {
    int neighbor;
    double ratio;
    ratio = (bin-basebins[row]) / (double) nBinsInRow[row];
    neighbor = ((int) round(ratio * nBinsInRow[row+distance]) + basebins[row+distance]);
    return neighbor;
}

/*
 * Function: getWindow
 * -------------------
 * Creates a n*n subset of a set of bins centered around a specified bin
 * bin: bin to center window on
 * row: row the center bin is in
 * width: dimension of window. it must be an odd number
 * data: data to subset
 * nBinsInRow: pointer to an array containing the number of bins in each row
 * basebins: pointer to an array containing the bin number of the first bin of each row
 * window: pointer to nxn 2-D array to write data values to
 */
void getWindow(int bin, int row, int width, const double* data, const int* nBinsInRow,
               const int* basebins, double** window) {
    int maxDistance = round(width/2);
    int nsNeighbor;
    for (int i = 0; i < width; i++) {
        nsNeighbor = getNeighborBin(bin, row, i - maxDistance, nBinsInRow, basebins);
        for (int j= 0; j < width; j++) {
            window[i][j] = data[nsNeighbor + (j - maxDistance) - 1];
        }
    }
}

double applyMedianFilter(double** window, int width) {
    double *flattened = (double *)malloc(sizeof(double)*(width*width));
    flatten2DArray(window, flattened, width, width);
    double mdn = median(flattened, (width * width));
    free(flattened);
    return mdn;
}

void contextualMedianFilter(int* bins, double* data, double* filteredData, int nbins, int nrows, int* nBinsInRow, int* basebins) {
    double **fiveWindow  = allocateMatrix(5,5);
    double **threeWindow = allocateMatrix(3,3);
    int row = 0;
    for (int i = 0; i < nbins; i++) {
        double value = data[i];
        if (bins[i] < basebins[row] + 2 || bins[i] > basebins[row+1] - 3 || row < 2 || row > nrows - 3) {
            if (i == basebins[row]+nBinsInRow[row] - 1) {
                row++;
            }
            filteredData[i] = value;
            continue;
        }
        getWindow(bins[i], row, 5, data, nBinsInRow, basebins, fiveWindow);
        getWindow(bins[i], row, 3, data, nBinsInRow, basebins, threeWindow);
        int isFivePeak = isWindowExtrema(5, fiveWindow);
        int isThreePeak = isWindowExtrema(3, threeWindow);
        if (isThreePeak && !isFivePeak) {
            value = applyMedianFilter(threeWindow, 3);
        }
        filteredData[i] = value;
        if ((i+1)/nBinsInRow[row] == 1) {
            row++;
        }
    }

    freeMatrix(fiveWindow,5);
    freeMatrix(threeWindow,3);
}
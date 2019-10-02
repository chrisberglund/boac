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
int isExtrema(int idx, const double arr[], int length) {
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
    int center = (int)(width-1)/2;
    double value = window[center][center];
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
 * Function: getThreeWindow
 * ------------------------
 * Gets a 3x3 subset of a 5x5 2-D array
 * :fiveWindow 5x5 2-D array
 * :threeWindow 3x3 2-D array to write subset to
 */
void getThreeWindow(double** fiveWindow, double** threeWindow) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            threeWindow[i][j] = fiveWindow[1 + i][1 + j];
        }
    }
};

/*
 *
 */
int getFiveWindow(int bin, int row, const double* data, const int* nBinsinRow,
                  const int basebins[], double** fiveWindow) {
    int distance = 2;
    int nsNeighbor;
    double ratio;
    for (int i = 0; i < 5; i++) {
        ratio = (bin-basebins[row]) / (double) nBinsinRow[row];
        nsNeighbor = ((int) round(ratio * nBinsinRow[row+(i-distance)]) + basebins[row+i-distance]);
        for (int j= 0; j < 5; j++) {
            fiveWindow[i][j] = data[nsNeighbor + (j - distance) - 1];
        }
    }
    return 1;
}

double applyMedianFilter(double** window, int width) {
    double *flattened = (double *)malloc(sizeof(double)*(width*width));
    flatten2DArray(window, flattened, width, width);
    double mdn = median(flattened, (width * width));
    free(flattened);
    return mdn;
}

double contextualMedianFilter(int* bins, double* data, int bin, int row, int* nBinsInRow, int* basebins) {
    double **fiveWindow  = allocateMatrix(5,5);
    double **threeWindow = allocateMatrix(3,3);
    double value = data[bin-1];
    getFiveWindow(bin, row, data, nBinsInRow, basebins, fiveWindow);
    getThreeWindow(fiveWindow, threeWindow);

    int isFivePeak = isWindowExtrema(5, fiveWindow);
    int isThreePeak = isWindowExtrema(3, threeWindow);

    if (isThreePeak && !isFivePeak) {
        value = applyMedianFilter(threeWindow, 3);
    }

    freeMatrix(fiveWindow,5);
    freeMatrix(threeWindow,3);

    return value;
}
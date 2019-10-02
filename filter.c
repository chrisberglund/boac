//
// Created by Christopher Berglund on 10/1/19.
//
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

int getThreeSlice(int fiveSlice[5][5], int threeSlice[][3]) {
    double oneDSlice[3];
    int i;
    for (i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            threeSlice[i][j] = fiveSlice[1 + i][1 + j];
        }
    }
    for (i = 0; i < 4; i++) {
        for (int j = 0; j < 3; j++) {
            switch(i) {
                //Northwest to Southeast slice
                case 0:
                    oneDSlice[j] = threeSlice[j][j];
                    break;
                    //North to South Slice
                case 1:
                    oneDSlice[j] = threeSlice[j][1];
                    break;
                    //Northeast to Southwest Slice
                case 2:
                    oneDSlice[j] = threeSlice[j][2-j];
                    break;
                case 3:
                    oneDSlice[j] = threeSlice[1][j];
                    break;
                default:
                    break;
            }
        }
    }
    return 1;
};

/*
 *
 */
int getFiveSlice(int* bins, int bin, int binIndex, int row, int nbins, int fiveSlice[5][5], int threeSlice[][3], const int* nBinsinRow,
                 const int basebins[]) {
    int distance = 2;
    int nsNeighbor;
    double ratio;
    for (int i = 0; i < 5; i++) {
        ratio = (bin-basebins[row]) / (double) nBinsinRow[row];
        nsNeighbor = ((int) round(ratio * nBinsinRow[row+(i-distance)]) + basebins[row+i-distance]);
        for (int j= 0; j < 5; j++) {
            fiveSlice[i][j] = nsNeighbor + (j - distance) - 1;
            if (fiveSlice[i][j] == -1) {
                return -1;
            }
        }
    }
    getThreeSlice(fiveSlice, threeSlice);
    return 1;
}
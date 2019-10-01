/*
 * Functions used to determines latitude, longitude, and arithmetic mean of bin data values
 * for NASA ocean level 3 binned data products
 */

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "helpers.h"


/*
 * Function: binarySearch
 * ----------------------
 * Returns the index to insert a value to maintain sort
 * arr: array to search
 * l: index to begin search at
 * r: max index to search at
 * x: value to search for
 * returns: the index at which to insert provided value in order to maintain sort
 */
int binarySearch(const int arr[], int l, int r, int x) {
    int mid = l + (r - l) / 2;
    if (r >= l) {
        if (arr[mid] == x)
            return mid;

        if (arr[mid] > x)
            return binarySearch(arr, l, mid - 1, x);

        return binarySearch(arr, mid + 1, r, x);
    }

    return -1;
}

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
void bin2latlon(int bin, int nBinsInRow[], double latrows[], int basebins[], int nrows, struct coordinates *coords) {
    if (bin < 1) {
        bin = 1;
    }
    int row = binarySearch(basebins, 0, nrows, bin) - 1;
    double clat = latrows[row];
    double clon = 360.0 * (bin - basebins[row] + 0.5) / nBinsInRow[row] - 180.0;
    coords->latitude = clat;
    coords->longitude = clon;
}

void test(int *bins, double *data, int nrows, int nbins, int *nbins_in_row, int *basebins) {
    int i, j;
    double **data_grid = (double **) malloc(nrows * sizeof(double *));
    for (i = 0; i < nrows; i++) {
        data_grid[i] = (double *) malloc(nbins_in_row[i] * sizeof(double));
    }
    int bin_index;
    for (i = 0; i < nrows; ++i) {
        for (j = 0; j < nbins_in_row[i]; ++j) {
            printf("%d", j);
            bin_index = binarySearch(bins, 0, nbins, basebins[i]);
            if (bin_index >= 0) {
                data_grid[i][j] = data[bin_index];
            }
        }
    }
    for (i = 0; i < nrows; i++) {
        free(data_grid[i]);
    }
    free(data_grid);
}

/*
 *
 */
int getFiveSlice(int bins[], int bin, int binIndex, int row, int nbins,
                 int neighborBins[5][5], const int *nBinsinRow, const int basebins[]) {
    int distance = 2;
    int neighborIndex;
    int nsNeighbor;
    double ratio;
    for (int i = 0; i < 5; i++) {
        ratio = (bin - basebins[row]) / (double) nBinsinRow[row];
        nsNeighbor = ((int) round(ratio * nBinsinRow[row + (i - distance)]) + basebins[row + i - distance]);
        for (int j = 0; j < 5; j++) {
            if (j == 2) {
                if (i == 2) {
                    neighborBins[i][j] = bin;
                } else if (i < 2) {
                    neighborIndex = binarySearch(bins, 0, binIndex, nsNeighbor);
                    neighborBins[i][j] = bins[neighborIndex];
                } else {
                    neighborIndex = binarySearch(bins, binIndex, nbins, nsNeighbor);
                    neighborBins[i][j] = bins[neighborIndex];
                }
            } else if (i == 2) {
                if (bins[binIndex + (j - distance)] == bin + (j - distance)) {
                    neighborBins[i][j] = binIndex + (j - distance);
                } else {
                    neighborBins[i][j] = -1;
                }
            } else {
                if (i < 2) {
                    neighborIndex = binarySearch(bins, 0, nbins, nsNeighbor + (j - distance));
                    neighborBins[i][j] = bins[neighborIndex];
                } else {
                    neighborIndex = binarySearch(bins, binIndex, nbins, nsNeighbor + (j - distance));
                    neighborBins[i][j] = bins[neighborIndex];
                }
            }
            if (neighborBins[i][j] == -1) {
                return -1;
            }
        }
    }
    return 1;
}
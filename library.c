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
int binary_search(int arr[], int l, int r, int x)
{
    int mid = l + (r - l) / 2;
    if (r >= l) {
        if (arr[mid] == x)
            return mid;

        if (arr[mid] > x)
            return binary_search(arr, l, mid - 1, x);

        return binary_search(arr, mid + 1, r, x);
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
void bin2latlon(int bin, int nBinsInRow[], double latrows[], int basebins[], int nrows, struct coordinates *coords)
{
    if (bin < 1) {
        bin = 1;
    }
    int row = binary_search(basebins, 0, nrows, bin) - 1;
    double clat = latrows[row];
    double clon = 360.0 * (bin - basebins[row] + 0.5) / nBinsInRow[row] - 180.0;
    coords -> latitude = clat;
    coords -> longitude = clon;
}

void test(int* bins, double* data, int nrows, int nbins, int* nbins_in_row, int* basebins)
{
    int i, j;
    double **data_grid = (double **)malloc(nrows * sizeof(double *));
    for (i = 0; i < nrows; i++) {
        data_grid[i] = (double *)malloc(nbins_in_row[i] * sizeof(double));
    }
    int bin_index;
    for (i=0; i<nrows; ++i) {
        for (j=0; j<nbins_in_row[i]; ++j) {
            printf("%d", j);
            bin_index = binary_search(bins, 0, nbins, basebins[i]);
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
 * Function: gridBins
 * ------------------
 * Determines latitude, longitude, and arithmetic mean data values for each bin
 * bins: array containing the id for all bins
 * data: array containing the weighted sum of the data value for each bin
 * weights: array containing weights for each bin
 * nbins: the total number of bins
 * nrows: the total number of rows
 * lats: output array to write latitude values to
 * lons: output array to write longitude values to
 * dataOut: output array to write mean data values to
 */
void gridBins(int* bins, double* data, double* weights,  int nbins, int nrows, double* lats, double* lons, double* dataOut)
{
    double *latlon;
    double latrows[nrows];
    int nbins_in_row[nrows];
    int basebins[nrows];
    int i;
    for (i=0; i < nrows; ++i) {
        latrows[i] = ((i + 0.5) * 180.0 / nrows) - 90;
        nbins_in_row[i] = 2 * nrows * cos(latrows[i] * M_PI / 180.0) + 0.5;
        if (i == 0) {
            basebins[i] = 1;
        } else {
            basebins[i] = basebins[i-1] + nbins_in_row[i-1];
        }
    }
    test(bins, data, nrows, nbins, nbins_in_row, basebins);
    struct coordinates *coords;
    coords = (struct coordinates *) malloc(sizeof(struct coordinates));
    for (i=0; i < nbins; ++i) {
        bin2latlon(bins[i], nbins_in_row, latrows, basebins, nrows, coords);
        lats[i] = coords -> latitude;
        lons[i] = coords -> longitude;
        dataOut[i] = data[i] / weights[i];
    }
    free(coords);
}
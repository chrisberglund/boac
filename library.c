/*
 * Functions used to determines latitude, longitude, and arithmetic mean of bin data values
 * for NASA ocean level 3 binned data products
 */

#include <stdio.h>
#include <math.h>
#include <stdlib.h>


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
int binarySearch(const int arr[], int l, int r, int x)
{
    int mid = l + (r - l) / 2;
    int test = arr[mid];
    if (r >= l) {
        if (arr[mid] == x) {

            return mid;
        }

        if (arr[mid] > x)
            return binarySearch(arr, l, mid - 1, x);

        return binarySearch(arr, mid + 1, r, x);
    }

    return -1;
}

int getThreeSlice(int fiveSlice[5][5], int threeSlice[][3]) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            threeSlice[i][j] = fiveSlice[1+i][1+j];
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

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



#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <assert.h>
#include "helpers.h"


/**
 * Finds the index of the closest value in a sorted array
 * @param arr sorted array
 * @param l index of the left most value of interest
 * @param r index of the right most value of interest
 * @param x value to search for
 * @return index of the closest value in the array
 */
int findClosestValue(int arr[], int l, int r, int x) {
    int mid = l + (r - l) / 2;
    if (r >= l) {
        if (arr[mid] == x)
            return mid;

        if (arr[mid] > x)
            return findClosestValue(arr, l, mid - 1, x);

        return findClosestValue(arr, mid + 1, r, x);
    }

    return mid;
}

void swap2(double *a, double *b) {
    double t = *a;
    *a = *b;
    *b = t;
}


int partition2(double *array, int left, int right, int pivot) {
    double pivotValue = array[pivot];
    swap2(&array[pivot], &array[right]);
    int storeIndex = left;
    for (int i = left; i < right; i++) {
        if (array[i] <= pivotValue) {
            swap2(&array[i], &array[storeIndex]);
            storeIndex++;
        }
    }
    swap2(&array[storeIndex], &array[right]);
    return storeIndex;
}

/**
 * Serial Quicksort implementation.
 */
void quicksort(double *array, int left, int right) {
    if (right > left) {
        int pivotIndex = left + (right - left) / 2;
        pivotIndex = partition2(array, left, right, pivotIndex);
        quicksort(array, left, pivotIndex - 1);
        quicksort(array, pivotIndex + 1, right);
    }
}

/**
 * Structure containing the arguments to the parallel_quicksort function.  Used
 * when starting it in a new thread, because pthread_create() can only pass one
 * (pointer) argument.
 */
struct qsort_starter {
    double *array;
    int left;
    int right;
    int depth;
};

/**
 * Thread trampoline that extracts the arguments from a qsort_starter structure
 * and calls parallel_quicksort.
 *//*
void *quicksort_thread(void *init) {
    struct qsort_starter *start = init;
    parallel_quicksort(start->array, start->left, start->right, start->depth);
    return NULL;
}
*/
/**
 * Parallel version of the quicksort function.  Takes an extra parameter:
 * depth.  This indicates the number of recursive calls that should be run in
 * parallel.  The total number of threads will be 2^depth.  If this is 0, this
 * function is equivalent to the serial quicksort.
 */
/*
void parallel_quicksort(double *array, int left, int right, int depth) {
   if (right > left) {
       int pivotIndex = left + (right - left) / 2;
       pivotIndex = partition(array, left, right, pivotIndex);
       // Either do the parallel or serial quicksort, depending on the depth
       // specified.
       if (depth-- > 0) {
           // Create the thread for the first recursive call
           struct qsort_starter arg = {array, left, pivotIndex - 1, depth};
           pthread_t thread;
           int ret = pthread_create(&thread, NULL, quicksort_thread, &arg);
           assert((ret == 0) && "Thread creation failed");
           // Perform the second recursive call in this thread
           parallel_quicksort(array, pivotIndex + 1, right, depth);
           // Wait for the first call to finish.
           pthread_join(thread, NULL);
       } else {
           quicksort(array, left, pivotIndex - 1);
           quicksort(array, pivotIndex + 1, right);
       }
   }
}
*/
double median(const double arr[], int length) {
    double *copy = (double *) malloc(length * sizeof(double));
    double median;
    bool isSorted = true;
    for (int i = 0; i < length; i++) {
        if (i > 0) {
            if (arr[i] < arr[i-1]) isSorted = false;
        }
        copy[i] = arr[i];
    }
    if (!isSorted)
        quicksort(copy, 0, length - 1);

    if (length % 2 == 0)
        median = (copy[((length - 1) / 2) - 1] + copy[(length - 1) / 2]) / 2;
    else
        median = copy[(int) round((length - 1) / 2.0)];

    free(copy);
    return median;
}

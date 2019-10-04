#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "helpers.h"

double **allocateMatrix(int r, int c) {
    double **arr = (double **) malloc(r * sizeof(double *));
    for (int i = 0; i < r; i++)
        arr[i] = (double *) malloc(c * sizeof(double));
    return arr;
}

void freeMatrix(double **arr, int r) {
    for (int i = 0; i < r; i++) {
        free(arr[i]);
    }
    free(arr);
}

void swap(double *a, double *b) {
    double t = *a;
    *a = *b;
    *b = t;
}

int partition(double arr[], int low, int high) {
    double pivot = arr[high];
    int i = (low - 1);

    for (int j = low; j <= high - 1; j++) {
        if (arr[j] < pivot) {
            i++;
            swap(&arr[i], &arr[j]);
        }
    }
    swap(&arr[i + 1], &arr[high]);
    return i + 1;
}

void quickSort(double* arr, int low, int high) {
    if (low < high) {
        int pi = partition(arr, low, high);
        quickSort(arr, low, pi - 1);
        quickSort(arr, pi + 1, high);
    }
}

/**
 * Takes 2-D array with given shape and write to 1-D array while maintaining order
 * @param arr pointer to 2-D array
 * @param out pointer to output array of length nrows * ncols
 * @param nrows number of rows in 2-D array
 * @param ncols number of columns in 2-D array
 */
void flatten2DArray(double** arr, double* out,int nrows, int ncols) {
    for (int i = 0; i < nrows; i++) {
        for (int j = 0; j < ncols; j++) {
            out[i * ncols + j] = arr[i][j];
        }
    }
}

double median(const double arr[], int length) {
    double *copy = (double *)malloc(length * sizeof(double));
    double median;
    for (int i = 0; i < length; i++) {
        copy[i] = arr[i];
    }

    quickSort(copy, 0, length - 1);
    if (length % 2 == 0) {
        median = (copy[((length-1) / 2) - 1] + copy[(length-1) / 2]) / 2;
    } else {
        median = copy[(int) round((length-1) / 2.0)];
    }
    free(copy);
    return median;
}

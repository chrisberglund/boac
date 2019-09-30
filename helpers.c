//
// Created by Christopher Berglund on 9/30/19.
//
#include <math.h>
#include "helpers.h"

void swap(double *a, double *b) {
    double t = *a;
    *a = *b;
    *b = t;
}

int partition(double arr[], int low, int high) {
    double pivot = arr[high];
    int i = (low - 1);

    for (int j = low; j <= high - 1; j++) {
        if (arr[j] < pivot)
        {
            i++;
            swap(&arr[i], &arr[j]);
        }
    }
    swap(&arr[i + 1], &arr[high]);
    return i + 1;
}
void quickSort(double arr[], int low, int high) {
    if (low < high) {
        int pi = partition (arr, low, high);
        quickSort(arr, low, pi - 1);
        quickSort(arr, pi + 1, high);
    }
}

double median(const double arr[], int length) {
    double copy[length];
    double median;
    for (int i = 0; i < length; i++) {
        copy[i] = arr[i];
    }
    quickSort(copy, 0, length-1);
    if (length % 2 == 0) {
        median = (copy[(length/2)-1] + copy[length/2])/2;
    } else {
        median = copy[(int)round(length/2)];
    }
    return median;
}

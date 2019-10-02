//
// Created by Christopher Berglund on 9/30/19.
//

#ifndef UNTITLED1_HELPERS_H
#define UNTITLED1_HELPERS_H
void quickSort(double arr[], int low, int high);
void flatten2DArray(double** arr, double* out,int nrows, int ncols);
double median(const double arr[], int length);
double **allocateMatrix(int r, int c);
void freeMatrix(double **arr, int n);
#endif //UNTITLED1_HELPERS_H

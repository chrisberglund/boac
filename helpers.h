#ifndef UNTITLED1_HELPERS_H
#define UNTITLED1_HELPERS_H
void quicksort(double arr[], int low, int high);
void flatten2DArray(double** arr, double* out,int nrows, int ncols);
double median(const double arr[], int length);
double **allocateMatrix(int r, int c);
void freeMatrix(double **arr, int n);
int findClosestValue(int arr[], int l, int r, int x);
void parallel_quicksort(double *array, int left, int right, int depth);
#endif //UNTITLED1_HELPERS_H

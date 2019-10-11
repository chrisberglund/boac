#include <stdbool.h>
#ifndef UNTITLED1_FILTER_H
#define UNTITLED1_FILTER_H

int isExtrema(int idx, const double arr[], int length);
int isWindowExtrema(int width, double* window);
bool getWindow(int bin, int row, int width, const double* data, const int* nBinsInRow,
              const int* basebins, double* window, double fillValue, bool fill);
void contextualMedianFilter(int* bins, double* data, double* filteredData, int nbins, int nrows,
                            int* nBinsInRow, int* basebins, double fillValue);

#endif //UNTITLED1_FILTER_H

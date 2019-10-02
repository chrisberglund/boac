//
// Created by Christopher Berglund on 10/1/19.
//

#ifndef UNTITLED1_FILTER_H
#define UNTITLED1_FILTER_H

int isExtrema(int idx, const double arr[], int length);
int isWindowExtrema(int width, double** window);
double contextualMedianFilter(int* bins, double* data, int bin, int row, int* nBinsInRow, int* basebins);

#endif //UNTITLED1_FILTER_H

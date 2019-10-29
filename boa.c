#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "prefilter.h"
#include "filter.h"
#include "threshold.h"
#include "sobel.h"

void boa(int totalBins, int nDataBins, int nrows, double fillValue,
         int *dataBins, int *rows, double *inData, double *weights, double *lats, double *lons, double *outData, bool chlora,
         bool glob) {
    int *nBinsInRow = (int *) malloc(sizeof(int) * nrows);
    int *basebins = (int *) malloc(sizeof(int) * nrows);
    int *bins = (int *) malloc(sizeof(int) * totalBins);
    double *data = (double *) malloc(sizeof(double) * totalBins);\
    if (glob) {
        createFullBinArrayGlob(totalBins, nDataBins,rows, nrows, dataBins, fillValue,
                               bins, inData,
                               lats, lons, nBinsInRow, basebins,
                               data, chlora);
    } else {
        createFullBinArray(totalBins, nDataBins, nrows, dataBins, fillValue,
                           bins, inData, weights,
                           lats, lons, nBinsInRow, basebins,
                           data, chlora);;
    }

    double *filteredData = (double *) malloc(sizeof(double) * totalBins);
    printf("Applying contextual median filter...");
    contextualMedianFilter(bins, data, filteredData, totalBins, nrows, nBinsInRow, basebins, fillValue);
    free(data);
    printf(" Finished! \n");
    double *magnitude = (double *) malloc(sizeof(double) * totalBins);
    printf("Applying sobel operator...");
    sobel(bins, filteredData, magnitude, totalBins, nrows, nBinsInRow, basebins, fillValue);
    free(filteredData);
    printf(" Finished! \n");
    printf("Thresholding \n");
    applyThreshold(bins, magnitude, outData, totalBins, nrows, nBinsInRow, basebins, fillValue);
    free(magnitude);
    free(nBinsInRow);
    free(basebins);
    free(bins);
    printf(" Finished! ]n");
}
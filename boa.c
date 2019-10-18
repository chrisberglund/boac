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
    double *data = (double *) malloc(sizeof(double) * totalBins);
    printf("%d", glob);
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
    contextualMedianFilter(bins, data, filteredData, totalBins, nrows, nBinsInRow, basebins, fillValue);
    free(data);
    double *magnitude = (double *) malloc(sizeof(double) * totalBins);
    sobel(bins, filteredData, magnitude, totalBins, nrows, nBinsInRow, basebins, fillValue);
    free(filteredData);
    applyThreshold(bins, magnitude, outData, totalBins, nrows, nBinsInRow, basebins, fillValue);
    free(magnitude);
    free(nBinsInRow);
    free(basebins);
    free(bins);
}
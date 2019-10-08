#include <stdlib.h>
#include <stdbool.h>
#include "prefilter.h"
#include "filter.h"
#include "sobel.h"

void boa(int totalBins, int nDataBins, int nrows, double fillValue,
         int *dataBins, double *inData, double *weights, double *lats, double *lons, double *outData, bool chlora) {
    int *nBinsInRow = (int *) malloc(sizeof(int) * nrows);
    int *basebins = (int *) malloc(sizeof(int) * nrows);
    int *bins = (int *) malloc(sizeof(int) * totalBins);
    double *data = (double *) malloc(sizeof(double) * totalBins);
    createFullBinArray(totalBins, nDataBins, nrows, dataBins, fillValue,
                       bins, inData, weights,
                       lats, lons, nBinsInRow, basebins,
                       data, chlora);

    double *filteredData = (double *) malloc(sizeof(double) * totalBins);
    contextualMedianFilter(bins, data, filteredData, totalBins, nrows, nBinsInRow, basebins, fillValue);
    free(data);

    sobel(bins, filteredData, outData, totalBins, nrows, nBinsInRow, basebins, fillValue);
    free(filteredData);
    free(nBinsInRow);
    free(basebins);
    free(bins);
}
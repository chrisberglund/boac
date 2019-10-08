#ifndef UNTITLED1_PREFILTER_H
#define UNTITLED1_PREFILTER_H

void createFullBinArray(int totalBins, int nDataBins, int nrows, const int *dataBins, double fillValue,
                        int *outBins, const double *inData, const double *weights,
                        double *lats, double *lons, int *nBinsInRow, int *basebins,
                        double *outData, bool chlora);

#endif //UNTITLED1_PREFILTER_H

//
// Created by Christopher Berglund on 10/10/19.
//

#ifndef BOAC_THRESHOLD_H
#define BOAC_THRESHOLD_H
void applyThreshold(int *bins, double *data, double *output, int nbins, int nrows, int *nBinsInRow,
                    int *basebins, double fillValue);
#endif //BOAC_THRESHOLD_H

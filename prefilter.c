#include "prefilter.h"

/**
 *
 * @param maxBins
 * @param nDataBins
 * @param dataBins
 * @param inData
 * @param outBins
 * @param outData
 */
void createFullBinArray(int maxBins, int nDataBins, int* dataBins, double* inData, int* outBins, double* outData) {
    for (int i = 0; i < maxBins; i++) {
        outBins[i] = i+1;
        outData[i] = -999.0;
    }
    for (int i = 0; i < nDataBins; i++) {
        outData[dataBins[i]-1] = inData[i];
    }
}

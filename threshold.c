#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <pthread.h>
#include "threshold.h"
#include "filter.h"
#include "helpers.h"
#include "quicksort.h"

/**
 * Gets the interquartile range
 * @param data pointer to a sorted array containing all the data values
 * @param nbins number of data values
 * @return interquartile range
 */
double iqr(const double *data, int nbins) {
    double *quartile = malloc(sizeof(double) * nbins/4);
    int firstq = (int)nbins/4;
    int secondq = (int)nbins/2;
    for (int i = 0; i < firstq; i++) {
        quartile[i] = data[i];
    }
    double firstmdn;
    if (nbins % 2 == 0)
        firstmdn = (quartile[((nbins/4 - 1) / 2) - 1] + quartile[(nbins/4 - 1) / 2]) / 2;
    else
        firstmdn = quartile[(int)((nbins/4.0 - 1) / 2.0)];
    for (int i = 0; i < firstq; i++) {
        quartile[i] = data[i+secondq];
    }
    printf("firstmdn: %f \n", firstmdn);
    double thirdmdn;
    if (nbins % 2 == 0)
        thirdmdn = (quartile[((nbins/4 - 1) / 2) - 1] + quartile[(nbins/4 - 1) / 2]) / 2;
    else
        thirdmdn = quartile[(int)((nbins/4.0 - 1) / 2.0)];
    printf("thirdmdn: %f \n", thirdmdn);
    free(quartile);
    return thirdmdn - firstmdn;
}

/**
 * Creates histogram of provided data values with bin width determined by Freedman–Diaconis rule
 * @param data pointer to a sorted array containing data to be made into histogram
 * @param nbins number of data values
 * @param h bin width
 * @param k number of bins
 * @param fillValue fill value
 * @return pointer to an array containing histogram frequencies. Caller is responsible for freeing memory
 */
double* getHistogram(const double *data, int nbins, double h, int k, double fillValue) {
    int *occurrence = malloc(sizeof(int) * k);
    double *histogram = malloc(sizeof(double) * k);
    for (int i = 0; i < k; i++) occurrence[i] = 0;
    for (int i = 0; i < k; i++) histogram[i] = 0;
    int ndatabins = 0;
    for (int i = 0; i < nbins; i++) {
        if (data[i] != fillValue) {
            int binValue = (int) (data[i]/h);
            occurrence[binValue] = occurrence[binValue] + 1;
            ndatabins++;
        }
    }
    for (int i = 0; i <= k; i++) {
        histogram[i] = (double) occurrence[i] / (double) ndatabins;
    }
    free(occurrence);
    return histogram;
}

double sumPOW(int *arr, double power, int length) {
    double sum = 0;
    for (int i = 0; i < length; i++) {
        sum += pow(arr[i],power);
    }
    return sum;
}
/*
double mse(int n, double h, int k, int *N) {
    return (2.0/(n-1)*h) - ((n+1)/(pow(n,2)*(n-1)*h)*sumPOW(N, 2, k));
}

double secant( double xA, double xB, double(*f)(double) )
{
    double e = 1.0e-12;
    double fA, fB;
    double d;
    int i;
    int limit = 50;

    fA=(*f)(xA);
    for (i=0; i<limit; i++) {
        fB=(*f)(xB);
        d = (xB - xA) / (fB - fA) * fB;
        if (fabs(d) < e)
            break;
        xA = xB;
        fA = fB;
        xB -= d;
    }
    if (i==limit) {
        printf("Function is not converging near (%7.4f,%7.4f).\n", xA,xB);
        return -99.0;
    }
    return xB;
}

double brent(double a, double b, double max, double min, int n, double *data, int ndata) {
    double step = 1.0e-2;
    double e = 1.0e-12;
    double h = -1.032;		// just so we use secant method
    double xx, value;
    int k = ceil((max-min)/h);
    int *occurrence = malloc(sizeof(int) * k);
    for (int i = 0; i < k; i++) occurrence[i] = 0;
    for (int i = 0; i < ndata; i++) {
            int binValue = (int) (data[i]/h);
            occurrence[binValue] = occurrence[binValue] + 1;
        }
    int s = (mse(n,h,k,occurrence)> 0.0);

    while (h < 1.0) {
        value = f(x);
        if (fabs(value) < e) {
            printf("Root found at x= %12.9f\n", x);
            s = (f(x+.0001)>0.0);
        }
        else if ((value > 0.0) != s) {
            xx = secant(x-step, x,&f);
            if (xx != -99.0)   // -99 meaning secand method failed
                printf("Root found at x= %12.9f\n", xx);
            else
                printf("Root found near x= %7.4f\n", x);
            s = (f(x+.0001)>0.0);
        }
        x += step;
    }
    return 0;
}*/
/**
 * Calculates the histogram bin width using the Freedman–Diaconis rule
 * @param data pointer to a sorted array containing all data values to histogram
 * @param nbins number of data values
 * @return optimal bin width for histogram
 */
double getBinWidth(double *data, int nbins) {
    double range = iqr(data, nbins);
    double h = 2 * range / cbrt(nbins);
    return h;
}

/**
 * Calculates optimal high threshold method using OTSU method
 * @param histogram histogram of data values
 * @param k number of bins
 * @return optimal high threshold value
 */
double otsuMethod(const double *histogram, int k, double h) {
    double *probability = malloc(sizeof(double) * k);
    double *mean = malloc(sizeof(double) * k);
    double *between = malloc(sizeof(double) * k);
    double max_between;
    double threshold;

    for (int i = 0; i < k; i++) {
        probability[i] = 0.0;
        mean[i] = 0.0;
        between[i] = 0.0;
    }
    probability[0] = histogram[0];

    for (int i = 1; i < k; i++) {
        probability[i] = probability[i - 1] + histogram[i];
        mean[i] = mean[i - 1] + i * histogram[i];
    }

    threshold = 0;
    max_between = 0.0;

    for (int i = 0; i < k; i++) {
        if (probability[i] != 0.0 && probability[i] != 1.0)
            between[i] = pow(mean[k-1] * probability[i] - mean[i], 2) / (probability[i] * (1.0 - probability[i]));
        else
            between[i] = 0.0;
        if (between[i] > max_between) {
            max_between = between[i];
            threshold = i * h;
        }
    }
    free(probability);
    free(mean);
    free(between);
    printf("threshold: %f \n", threshold);
    return threshold;
}



struct Node {
    int data;
    struct Node* next;
};

/**
 * Calculates the ideal high threshold value using OTSU method TODO: Separate out some of linked list implementation and handle too large values
 * @param data pointer to an array containing all data values
 * @param nbins number of data values
 * @param fillValue fill value
 * @return ideal high threshold value
 */
double getThreshold(const double *data, int nbins, double fillValue) {
    struct Node* head = NULL;
    struct Node* tail = NULL;
    struct Node* tmp = NULL;

    head = (struct Node*) malloc(sizeof(struct Node));
    tail = head;
    int nValidBins = 0;
    for (int i = 0; i < nbins; i++) {
        if (data[i] != fillValue && data[i] < 3.0) {
            tail->data = i;
            tmp = (struct Node *) malloc(sizeof(struct Node));
            tail->next = tmp;
            tail = tmp;
            nValidBins++;
        }
    }
    if (nValidBins == 0) {
        free(head);
        return -1*fillValue;
    }
    double *copy = (double *) malloc(sizeof(double) * nValidBins);
    for (int i = 0; i < nValidBins; i++) {
        copy[i] = data[head->data];
        tmp = head;
        head = head->next;
        free(tmp);
    }
    sort(copy, nValidBins);
    double h = getBinWidth(copy, nValidBins);
    h = 0.1;
    int k = (int)((copy[nValidBins-1] - copy[0]) / h);
    double *histogram = getHistogram(copy, nValidBins, h, k, fillValue);
    double threshold = otsuMethod(histogram, k, h);
    free(histogram);
    free(copy);
    return threshold;
}

/**
 * Determines if a pixel is above the lower threshold or the higher threshold
 * @param value pixel value of interest
 * @param lowThreshold lower threshold to check pixel value against
 * @param highThreshold higher threshold to check pixel value against
 * @param maxValid maximum valid value
 * @return 2 if pixel is above high threshold, 1 if pixel is above low threshold, and 0 if it is below both thresholds
 */
double testThreshold(double value, double lowThreshold, double highThreshold, double maxValid) {
    if (value > highThreshold && value <= maxValid) {
        return 2.;
    } else if (value > lowThreshold && value <= maxValid) {
        return 1.;
    } else {
        return 0.;
    }
}

/**
 * Checks if pixel is either a high pixel or adjacent to a high pixel for hysteresis
 * @param window array of width*width length with pixel of interest at center
 * @param width width of window
 * @return 1 if pixel is connected to or is a high pixel, 0 otherwise
 */
bool isConnected(const double *window, int width) {
    int center = width * (int) ((width - 1) / 2.0) + (int) ((width - 1) / 2.0);
    if (window[center] == 2) {
        return true;
    }
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < width; j++) {
            if (window[i * width + j] == 2 && window[center] >= 1) {
                return true;
            }
        }
    }
    return false;
}

void applyThreshold(int *bins, double *data, double *output, int nbins, int nrows, int *nBinsInRow,
                    int *basebins, double fillValue) {
    int row = 0;
    double *threshold = (double *) malloc(sizeof(double) * nbins);
    double *window = (double *) malloc(sizeof(double) * 9);
    double high = 1.5;
    double low = 0.5 * high;
    for (int i = 0; i < nbins; i++) {
        if (i == basebins[row] + nBinsInRow[row]) {
            row++;
        }
        if (row < 2 || row > nrows - 3) {
            output[i] = fillValue;
            continue;
        }
        threshold[i] = testThreshold(data[i], low, high, 256.0);
        bool isValid = getWindow(bins[i], row, 3, threshold, nBinsInRow, basebins, window, fillValue, false);
        if (data[i] == fillValue) {
            output[i] = fillValue;
        } else if (isConnected(window, 3) && isValid) {
            output[i] = 1;
        } else if (isValid) {
            output[i] = 0;
        }
    }
    free(window);
    free(threshold);
}


#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>

// cc -O3 -lpthread
int max_threads;
int n_threads;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;


void qusort(double *left, double *right);

#define swap(a, b) { double _h = a; a = b; b = _h; }

void insert_sort(double *left, double *right) {

    for (double *pi = left + 1; pi <= right; pi++) {
        if (*pi < *left) {
            swap(*pi, *left);
        }
    }
    for (double *pi = left + 2; pi <= right; pi++) {
        double h = *pi;
        double *pj = pi - 1;
        while (h < *pj) {
            *(pj + 1) = *pj;
            pj -= 1;
        }
        *(pj + 1) = h;
    }
}

void sort3fast(double a, double b, double c) {
    if (b < a) {
        if (c < a) {
            if (c < b) {swap(a, c); }
            else {
                double h = a;
                a = b;
                b = c;
                c = h;
            }
        } else {swap(a, b); }
    } else {
        if (c < b) {
            if (c < a) {
                double h = c;
                c = b;
                b = a;
                a = h;
            } else {swap(b, c); }

        }

    }
}


void partition(double* left0, double* right0, double** l1, double** r1, double** l2, double** r2) {

    double* left = left0 + 1;
    double* right = right0;

    double* mid = left0 + (right0 - left0) / 2;
    double piv = *mid;
    *mid = *left;
    sort3fast(*left0, piv, *right0);
    *left = piv;

    while (1) {
        do left += 1; while(*left < piv);
        do right -= 1; while (*right > piv);
        if (left >= right) break;
        swap(*left, *right);
    }
    *(left0 + 1) = *right;
    *right = piv;

    if (right < mid) {
        *l1 = left0; *r1 = right - 1;
        *l2 = right + 1; *r2 = right0;
    }
    else {
        *l1 = right + 1; *r1 = right0;
        *l2 = left0; *r2 = right - 1;
    }
}

void *sort_thr(void *arg) {
    double **par = (double **) arg;
    qusort(par[0], par[1]);
    free(arg);
    pthread_mutex_lock(&mutex);
    n_threads -= 1;
    if (n_threads == 0) pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);
    return NULL;
}

void qusort(double *left, double *right) {

    while (right - left >= 50) {
        double *l, *r;
        partition(left, right, &l, &r, &left, &right);

        if (right - left > 25000 && n_threads < max_threads) {
            // start a new thread - max_threads is a soft limit
            pthread_t thread;
            double **param = malloc(2 * sizeof(double *));
            param[0] = left;
            param[1] = right;
            pthread_mutex_lock(&mutex);
            n_threads += 1;
            pthread_mutex_unlock(&mutex);
            pthread_create(&thread, NULL, sort_thr, param);
            left = l;
            right = r;
        } else {
            qusort(l, r);
        }
    }
    insert_sort(left, right);
}

void sort(double *data, int len) {

    int n_cpus = sysconf(_SC_NPROCESSORS_ONLN);
    if (n_cpus > 0) max_threads = n_cpus * 2;
    else max_threads = 8;

    pthread_t thread;
    double **param = malloc(2 * sizeof(double *));
    param[0] = data;
    param[1] = data + len - 1;
    n_threads = 1;
    pthread_create(&thread, NULL, sort_thr, param);

    pthread_mutex_lock(&mutex);
    pthread_cond_wait(&cond, &mutex);
    pthread_mutex_unlock(&mutex);
}
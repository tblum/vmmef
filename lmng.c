//
// Created by troels on 9/6/16.
//
/*
 * Levenberg–Marquardt fit of sum of two normal distributions.
 * Using lmfit: http://apps.jcns.fz-juelich.de/doku/sc/lmfit
 * Implementation inspired by lmcurve from the same project.
 */

#include <stddef.h>
#include "lmng.h"

void lmng_eval(const double *par,  // function parameters: mu1, sd1, mu2, sd2 ...
               const int n_dat,    // Size of fvec: Number of datapoints
               const void *_data,  // {n,ig,x0,y[]}
               double *fvec,       // result vector
               int *info)
{
    lmng_data_t* data = (lmng_data_t*)_data;
    size_t n = data->n;
    double ig = (double)(data->ig);
    ig = ig/(double)(n);
    long x0 = data->x0;
    for (int i = 0; i < n_dat; ++i)
    {
        double x = (double)(x0+i);
        double sum = 0.0;
        for (size_t gs = 0; gs < n; ++gs)
        {
            size_t pi = gs*NG_PAR;
            sum += g(par[pi], par[pi+1], ig, x);
        }
        fvec[i] = (double)(data->y[i]) - sum;
    }
}

void lmngi_eval(const double *par,  // function parameters: mu1, sd1, ig1, mu2, sd2, ig2 ...
                const int n_dat,    // Size of fvec: Number of datapoints
                const void *_data,  // {n,ig,x0,y[]}
                double *fvec,       // result vector
                int *info)
{
    lmng_data_t* data = (lmng_data_t*)_data;
    size_t n = data->n;
    long x0 = data->x0;
    for (int i = 0; i < n_dat; ++i)
    {
        double x = (double)(x0+i);
        double sum = 0.0;
        for (size_t gs = 0; gs < n; ++gs)
        {
            size_t pi = gs*NGI_PAR;
            sum += g(par[pi], par[pi+1], par[pi+2], x);
        }
        fvec[i] = (double)(data->y[i]) - sum;
    }
}

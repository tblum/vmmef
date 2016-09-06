//
// Created by troels on 9/6/16.
//
#include <math.h>
#include <stddef.h>
#include "lmdg.h"

double inline g(double i, double x, double m, double sd)
{
    double xm = x*m; double sd2 = 2*sd;
    return (i/(sd*sqrt(2*M_PI)))*exp(-(xm*xm)/(sd2*sd2));
}

void lmdg_evaluate(const double par[DG_PAR], // function parameters: mu1, sd1, mu2, sd2
                   const int n_dat,          // Size of fvec: Number of datapoints
                   const void* _data,         // {ig,x,y[]}
                   double* fvec,             // result vector
                   int* info)
{
    lmdg_data_t* data = (lmdg_data_t*)_data;
    double ig = (double)(data->ig/2);
    long x0 = data->x0;
    double m1 = par[0];
    double sd1 = par[1];
    double m2 = par[2];
    double sd2 = par[3];

    for (int i = 0; i < n_dat; ++i)
    {
        double x = (double)(x0+i);
        fvec[i] = data->y[i] - (g(ig, x, m1, sd1) + g(ig, x, m2, sd2));
    }
}

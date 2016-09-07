//
// Created by troels on 9/6/16.
//
/*
 * Levenberg–Marquardt fit of sum of two normal distributions.
 * Using lmfit: http://apps.jcns.fz-juelich.de/doku/sc/lmfit
 * Implementation inspired by lmcurve from the same project.
 */

#ifndef VMMEF_LMDG_H
#define VMMEF_LMDG_H

#include <lmmin.h>
#define DG_PAR 4

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    const long ig;
    const long x0;
    const int* y;
} lmdg_data_t;

void lmdg_eval(const double par[DG_PAR], // function parameters: mu1, sd1, mu2, sd2
               const int m_dat,          // Size of fvec: Number of datapoints
               const void* data,         // {ig,x,y[]}
               double* fvec,             // result vector
               int* info);

#ifdef __cplusplus
}
#endif
#endif //VMMEF_LMDG_H

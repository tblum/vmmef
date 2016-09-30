//
// Created by troels on 9/6/16.
//
/*
 * Levenberg–Marquardt fit of sum of several normal distributions.
 * Using lmfit: http://apps.jcns.fz-juelich.de/doku/sc/lmfit
 * Implementation inspired by lmcurve from the same project.
 */

#ifndef VMMEF_LMNG_H
#define VMMEF_LMNG_H

#include <math.h>
#include <lmmin.h>
#include <float.h>

#define NG_PAR 2
#define NGI_PAR 3

#ifdef __cplusplus
extern "C" {
#endif
static inline double g(double m, double sd, double i, double x)
{
    if (sd < 1.0 || i < 100)
        return -FLT_MAX;
    double dxm = x-m;
    return (i/(sd*sqrt(2*M_PI)))*exp(-(dxm*dxm)/(2*sd*sd));
}

typedef struct
{
    const size_t n; // Number of gusian functions to sum
    const long ig;  // Integral of the function
    const long x0;  // x offset
    const int* y;   // measured data
} lmng_data_t;

// Eval function for fitting n gauss functions with fixed integral: ig/n
void lmng_eval(const double *par,
               const int m_dat,
               const void *data,
               double *fvec,
               int *info);

// Eval function for fitting n gauss functions where the integral of the single gauss function is also a free variable
void lmngi_eval(const double *par,
                const int n_dat,
                const void *_data,
                double *fvec,
                int *info);


#ifdef __cplusplus
}
#endif
#endif //VMMEF_LMNG_H

//
// Created by troels on 9/5/16.
//

#ifndef VMMEF_DOUBLEGAUSSFIT_H
#define VMMEF_DOUBLEGAUSSFIT_H

#include "lmdg.h"

struct GaussParam {double mean; double sigma;};

class DoubleGaussFit
{
private:
    DoubleGaussFit() {};
public:
    GaussParam gauss1;
    GaussParam gauss2;
    DoubleGaussFit(const int* hist, size_t n_hist, long x0, double sum, long N, double variance);
};


#endif //VMMEF_DOUBLEGAUSSFIT_H

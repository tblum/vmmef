//
// Created by troels on 9/5/16.
//

#include <cmath>
#include "DoubleGaussFit.h"
#include "lmdg.h"

DoubleGaussFit::DoubleGaussFit(const int* hist, size_t n_hist, long x0, double sum, long N, double variance)
{
    lmdg_data_t data = { N, x0, hist};
    double par[DG_PAR];
    double mean = sum/N;
    double sd = std::sqrt(variance);
    par[0] = mean-sd;
    par[1] = sd*2;
    par[2] = mean+sd;
    par[3] = sd/2;
    lm_control_struct control = lm_control_double;
    lm_status_struct status;
    lmmin(DG_PAR, par, n_hist, (const void*)&data, &lmdg_eval, &control, &status);
    gauss1.mean  = par[0];
    gauss1.sigma = par[1];
    gauss2.mean  = par[2];
    gauss2.sigma = par[3];
}

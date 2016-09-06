//
// Created by troels on 9/5/16.
//

#include <cmath>
#include "DoubleGaussFit.h"
#include "lmdg.h"

DoubleGaussFit::DoubleGaussFit(const int* hist, size_t n_hist, long x0, double sum, long N, double variance)
{
    lmdg_data_t data = {N, x0, hist};
    double par[DG_PAR];
    double mean = sum/N;
    double sd = std::sqrt(variance);
    par[0] = mean;
    par[1] = sd;
    par[2] = mean;
    par[3] = sd;
    lm_control_struct control = lm_control_double;
    lm_status_struct status;
    lmmin(DG_PAR, par, n_hist, (const void*)&data, &lmdg_eval, &control, &status);
}

//
// Created by troels on 8/30/16.
//

#ifndef VMMEF_PEDISTAL_H
#define VMMEF_PEDISTAL_H

#include "FECEvent.h"

#define DG_PARAMETERS 5
// We ignore data points outside [PED_MIN:PED_MAX[

class Pedestal
{
private:
    int* histogram;
    struct Tally {long N; double sum;};
    struct GaussParam {double mean; double sigma;};
    Tally tally(const int hist[APV_MAX_VALUE], size_t begin, size_t end) const;
    Tally tally(const int hist[APV_MAX_VALUE]) const {return tally(hist,0,APV_MAX_VALUE);};
    double variance(const int hist[APV_MAX_VALUE], Tally t, size_t begin, size_t end) const;
    double variance(const int hist[APV_MAX_VALUE], Tally t) const {return variance(hist,t,0,APV_MAX_VALUE);};
    std::pair<GaussParam, GaussParam> doubleGaussFit();
protected:
    void printOn(std::ostream& os) const;
public:
    Pedestal();
    ~Pedestal();
    void printStats(std::ostream &os) const;
    void printGPdata(std::ostream &os) const;
    void printGPscript(std::ostream &os, std::string dataFile, double bsig, double esig) const;
    void add(ADCiterator& begin);
    friend std::ostream& operator<< (std::ostream& os, Pedestal const& p);
};


#endif //VMMEF_PEDISTAL_H

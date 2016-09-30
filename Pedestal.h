//
// Created by troels on 8/30/16.
//

#ifndef VMMEF_PEDISTAL_H
#define VMMEF_PEDISTAL_H

#include <vector>
#include "FECEvent.h"
#include "lmng.h"

#define DG_PARAMETERS 5
// We ignore data points outside [PED_MIN:PED_MAX[

struct GaussParam {double mean; double sigma; double ig;};

class Pedestal
{
private:
    int* histogram;

    struct Tally {long N; double sum;};
    Tally tally(const int hist[APV_MAX_VALUE], size_t begin, size_t end) const;
    Tally tally(const int hist[APV_MAX_VALUE]) const {return tally(hist,0,APV_MAX_VALUE);};
    double variance(const int hist[APV_MAX_VALUE], Tally t, size_t begin, size_t end) const;
    double variance(const int hist[APV_MAX_VALUE], Tally t) const {return variance(hist,t,0,APV_MAX_VALUE);};
    std::pair<size_t , size_t > findCutoff(const int hist[APV_MAX_VALUE],size_t o, size_t n, int v) const;
    std::vector<GaussParam> gaussFit(const int* hist, size_t hist_size, long x0, double sum, long N,
                                     double variance, size_t ng, lm_status_struct* status, bool freeIntegral) const;
protected:
    void printOn(std::ostream& os) const;
public:
    Pedestal();
    ~Pedestal();
    void printStats(std::ostream &os) const;
    void printGPdata(std::ostream &os) const;
    void printGPscript(std::ostream &os, std::string dataFile, int id, size_t curves,
                       bool freeIntegral, bool fixedIntegral) const;
    void add(ADCiterator& begin);
    friend std::ostream& operator<< (std::ostream& os, Pedestal const& p);
    void finalize();
    uint16_t cutOff(size_t c) ;
};


#endif //VMMEF_PEDISTAL_H

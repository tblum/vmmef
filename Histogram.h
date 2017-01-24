//
// Created by troels on 10/4/16.
//

#ifndef VMMEF_HISTOGRAM_H
#define VMMEF_HISTOGRAM_H

#include <lmstruct.h>
#include <vector>
#include "FECEvent.h"
#include "SRSRawFile.h"
#include "HDF5File.h"

class Histogram
{
private:
    uint* data;                   //histogram raw data[0..511][apv_value] = x[0..255][], y[0..255][]

    struct Tally {long long N; double sum;};
    struct GaussParam {double mean; double sigma; double ig;};
    Tally tally(const uint hist[APV_MAX_VALUE], size_t begin, size_t end) const;
    Tally tally(const uint hist[APV_MAX_VALUE]) const {return tally(hist,0,APV_MAX_VALUE);};
    double variance(const uint hist[APV_MAX_VALUE], Tally t, size_t begin, size_t end) const;
    double variance(const uint hist[APV_MAX_VALUE], Tally t) const {return variance(hist,t,0,APV_MAX_VALUE);};
    std::pair<uint16_t , uint16_t > findCutoffPair (const uint *hist, uint16_t o, uint16_t n, uint v) const;
    std::vector<GaussParam> gaussFit(const uint* hist, size_t hist_size, long x0, double sum, long N,
                                     double variance, size_t ng, lm_status_struct* status, bool freeIntegral) const;
    void build(SRSRawFile& file, int n);
    inline void inc(size_t strip_id, size_t value) { data[strip_id*APV_MAX_VALUE + value] +=1; }
    inline const uint* channel(size_t strip_id) const { return data + strip_id*APV_MAX_VALUE; }
    uint16_t findCutoff (const uint *hist, uint16_t o, uint16_t n, uint v) const;

public:
    Histogram(SRSRawFile& file, int n);
    ~Histogram();
    uint16_t findCutoff (size_t strip_id, uint16_t o, uint16_t n, uint v) const
    {return findCutoff(channel(strip_id), o, n, v);}
    double average(size_t strip_id) const;
    friend void FECEvent::addToHistogram (Histogram *histogram) const;
    friend void HDF5File::addPedestal(Pedestal* pedestal, Histogram *histogram);
};


#endif //VMMEF_HISTOGRAM_H

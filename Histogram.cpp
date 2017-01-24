//
// Created by troels on 10/4/16.
//

#include <ctime>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <iostream>
#include "SRSRawFile.h"
#include "FECEvent.h"
#include "Histogram.h"
#include "vmmef.h"

Histogram::Histogram(SRSRawFile& file, int n=-1)
{
    size_t size = DAQ_CHANNELS*APV_CHANNELS*APV_MAX_VALUE;
    data = new uint[size];      // One malloc
    memset(data,0,size*sizeof(uint)); // Initialize histogram
    build(file, n);
}

Histogram::~Histogram()
{
    delete[] data;
}

// Function to build the histogram
void Histogram::build(SRSRawFile& file, int n)
{
    int i = 0;
    while(++i)
    {
        try
        {
            FECEvent event = file.nextFECEvent();
            event.addToHistogram(this);
        }
        catch (const std::exception& e) // TODO only catch EOF
        {
            break;
        }
        if (n > 0 && i > n)
            break;
    }
}

Histogram::Tally Histogram::tally(const uint hist[APV_MAX_VALUE], size_t begin, size_t end) const
{
    double sum = 0;
    long long N = 0;
    for(size_t v = begin; v < end; ++v)
    {
        int h = hist[v];
        sum += (double)(h*v);
        N += h;
    }
    Tally t = {N,sum};
    return t;
}

double Histogram::average(size_t strip_id) const
{
    const uint* hist = channel(strip_id);
    Histogram::Tally t = tally(hist);
    return (t.sum/t.N);
}


double Histogram::variance(const uint *hist, Tally t, size_t begin, size_t end) const
{
    double var = 0.0;
    double mean = t.sum / (double)t.N;
    for(size_t v = begin; v < end; ++v)
    {
        double d = ((double)v - mean);
        var += d*d*hist[v];
    }
    return (var / (double)t.N);
}

uint16_t Histogram::findCutoff (const uint *hist,  // 1D histogram
                                uint16_t o,    // point to search from - typically average value
                                uint16_t n,    // Number of bins to find under value v
                                uint v) const // Max bin value
{
    uint16_t end = o-1;
    size_t ne = 0;
    while (end < APV_MAX_VALUE && ne < n)
    {
        if (hist[++end] < v)
            ++ne;
        else
            ne = 0;
    }
    return end-n;
}

std::pair<uint16_t , uint16_t > Histogram::findCutoffPair (const uint *hist,  // 1D histogram
                                                           uint16_t o,    // point to search from - typically average value
                                                           uint16_t n,    // Number of bins to find under value v
                                                           uint v) const // Max bin value
{
    uint16_t start = o+1;
    size_t ns = 0;
    while (start > 0 && ns < n)
    {
        if (hist[--start] < v)
            ++ns;
        else
            ns = 0;
    }
    uint16_t end = o-1;
    size_t ne = 0;
    while (end < APV_MAX_VALUE && ne < n)
    {
        if (hist[++end] < v)
            ++ne;
        else
            ne = 0;
    }
    return std::make_pair(start,end);
}



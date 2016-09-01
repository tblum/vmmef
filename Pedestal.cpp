//
// Created by troels on 8/30/16.
//

#include <iomanip>
#include "Pedestal.h"

Pedestal::Pedestal ()
        : dataPoints(0)
{
    for (int i = 0; i < APV_CHANNELS; ++i)
    {
        sum[i] = 0.0;
        min[i] =  APV_MAX_VALUE-1;
        max[i] = 0;
        for (int j = 0; j < APV_MAX_VALUE; ++j)
        {
            hist[i][j] = 0;
        }
    }
}

void Pedestal::add (ADCiterator &it)
{
    for (size_t i = 0; i < APV_TIME_BINS; ++i)
    {
        for (size_t j = 0; j < APV_CHANNELS; ++j)
        {
            uint16_t v = (APV_MAX_VALUE - (*it++));
            sum[j] += (double)(v);
            min[j] = std::min(min[j],v);
            max[j] = std::max(max[j],v);
            hist[j][v] += 1;
        }
        ++dataPoints;
        it += APV_HEADER_BITS;
    }
}

double variance(const int hist[APV_MAX_VALUE], double sum, int n)
{
    double v = 0.0;
    double mean = sum / (double)n;
    for(int i = 0; i < APV_MAX_VALUE; ++i)
    {
        double d = ((double)i - mean);
        v += d*d*hist[i];
    }
    return (v / (double)n);
}

void Pedestal::printStats (std::ostream &os) const
{
    for (int i = 0; i < APV_CHANNELS; ++i)
    {
        os << "Chan " << std::setw(4) << i <<
           ": min " << std::setw(6) << min[i] <<
           ", max " << std::setw(6) << max[i] <<
           ", avg " << std::setw(6) << (int)(sum[i]/dataPoints) <<
           ", var " << std::setprecision(5) << variance(hist[i], sum[i], dataPoints) << std::endl;
    }
}

void Pedestal::printOn (std::ostream &os) const
{
    for (int i = 0; i < APV_CHANNELS; ++i)
    {
        os << hist[i][0];
        for (int j = 1; j < APV_MAX_VALUE; ++j)
        {
            os << "," << hist[i][j];
        }
        os << "\n";
    }
}

std::ostream& operator<< (std::ostream& os, Pedestal const& p)
{
    p.printOn(os);
    return os;
}

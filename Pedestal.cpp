//
// Created by troels on 8/30/16.
//

#include <iomanip>
#include <cmath>
#include "Pedestal.h"

Pedestal::Pedestal ()
        : dataPoints(0)
{
    for (size_t c = 0; c < APV_CHANNELS; ++c)
    {
        sum[c] = 0.0;
        min[c] =  APV_MAX_VALUE-1;
        max[c] = 0;
        for (size_t v = 0; v < APV_MAX_VALUE; ++v)
        {
            hist[c][v] = 0;
        }
    }
}

void Pedestal::add (ADCiterator &it)
{
    for (size_t tb = 0; tb < APV_TIME_BINS; ++tb)
    {
        for (size_t c = 0; c < APV_CHANNELS; ++c)
        {
            uint16_t v = (APV_MAX_VALUE - (*it++));
            sum[c] += (double)(v);
            min[c] = std::min(min[c],v);
            max[c] = std::max(max[c],v);
            hist[c][v] += 1;
        }
        ++dataPoints;
        it += APV_HEADER_BITS;
    }
}

double variance(const int hist[APV_MAX_VALUE], double sum, int n)
{
    double var = 0.0;
    double mean = sum / (double)n;
    for(size_t v = 0; v < APV_MAX_VALUE; ++v)
    {
        double d = ((double)v - mean);
        var += d*d*hist[v];
    }
    return (var / (double)n);
}

void Pedestal::printStats (std::ostream &os) const
{
    for (size_t c = 0; c < APV_CHANNELS; ++c)
    {
        os << "Chan " << std::setw(4) << c <<
           ": min " << std::setw(6) << min[c] <<
           ", max " << std::setw(6) << max[c] <<
           ", avg " << std::setw(6) << (int)(sum[c]/dataPoints) <<
           ", var " << std::setprecision(5) << variance(hist[c], sum[c], dataPoints) << std::endl;
    }
}

void Pedestal::printOn (std::ostream &os) const
{
    for (size_t c = 0; c < APV_CHANNELS; ++c)
    {
        os << hist[c][0];
        for (size_t v = 1; v < APV_MAX_VALUE; ++v)
        {
            os << "," << hist[c][v];
        }
        os << "\n";
    }
}

void Pedestal::printGPdata (std::ostream &os) const
{
    os << "# Value";
    for (size_t c = 0; c < APV_CHANNELS; ++c)
    {
        os << " c-" << std::setfill('0') << std::setw(3) << c << std::setfill(' ');
    }
    os << "\n";
    for (size_t v = 0; v < APV_MAX_VALUE; ++v)
    {
        os << std::setw(7) << v;
        for (size_t c = 0; c < APV_CHANNELS; ++c)
        {
            os << " " << std::setw(5) << hist[c][v];
        }
        os << "\n";
    }
}

void Pedestal::printGPscript (std::ostream &os, std::string dataFile, double nsig) const
{
    std::string baseName = dataFile.substr(0,dataFile.find_last_of("."));
    os << "# gnuplot script for plotting APV pedestal histograms\n" <<
       "normal(x, s, mu, sd) = (s/(sd*sqrt(2*pi)))*exp(-(x-mu)**2/(2*sd**2))\n"
       "set style data histogram\n" <<
       "set style histogram cluster gap 1\n" <<
       "set terminal svg size 1200,960 enhanced background rgb \'white\'\n" <<
       "set style fill solid 1.0\n" <<
       "#set logscale y\n";
    for (size_t c = 0; c < APV_CHANNELS; ++c)
    {
        double avg = sum[c]/dataPoints;
        double var = variance(hist[c], sum[c], dataPoints);
        double sigma = std::sqrt(var);
        os << "set xrange [" << (int)(avg - nsig*sigma) << ":" << (int)(avg + nsig*sigma) << "]\n" <<
           "set output \'" << baseName << c << ".svg\'\n" <<
           "plot \"" << dataFile << "\" using " << c+2 << " linecolor rgb \"#888888\",\\\n" <<
           "\tnormal (x," << dataPoints << ","  << avg << "," << sigma << ") linecolor rgb \"#AA0000\"\n";
    }
}

std::ostream& operator<< (std::ostream& os, Pedestal const& p)
{
    p.printOn(os);
    return os;
}


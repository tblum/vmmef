//
// Created by troels on 8/30/16.
//

#include <iomanip>
#include <cmath>
#include <vector>
#include <cstring>
#include <iostream>
#include "Pedestal.h"

#define HIST(C,V) histogram[(C)*(APV_MAX_VALUE) + (V)]
#define HISTC(C) &histogram[(C)*(APV_MAX_VALUE)]

Pedestal::Pedestal ()
{
    histogram = new int[APV_CHANNELS*APV_MAX_VALUE];
    memset(histogram,0,APV_CHANNELS*(APV_MAX_VALUE)*sizeof(int));
}

Pedestal::~Pedestal ()
{
    delete[] histogram;
}

void Pedestal::add (ADCiterator &it)
{
    for (size_t tb = 0; tb < APV_TIME_BINS; ++tb)
    {
        for (size_t c = 0; c < APV_CHANNELS; ++c)
        {
            uint16_t v = (APV_MAX_VALUE - (*it++));
            HIST(c,v) += 1;
        }
        it += APV_HEADER_BITS;
    }
}

double Pedestal::variance (const int *hist, Tally t, size_t begin, size_t end) const
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

Pedestal::Tally Pedestal::tally(const int hist[APV_MAX_VALUE], size_t begin, size_t end) const
{
    double sum = 0.0;
    long N = 0;
    for(size_t v = begin; v < end; ++v)
    {
        int h = hist[v];
        sum += (double)(h*v);
        N += h;
    }
    Tally t = {N,sum};
    return t;
}

void Pedestal::printStats (std::ostream &os) const
{
    for (size_t c = 0; c < APV_CHANNELS; ++c)
    {
        Tally t = tally(HISTC(c));
        os << "Chan " << std::setw(4) << c <<
           ", avg " << std::setw(6) << (int)(t.sum/t.N) <<
           ", var " << std::setprecision(5) << variance(HISTC(c), t) << std::endl;
    }
}

void Pedestal::printOn (std::ostream &os) const
{
    for (size_t c = 0; c < APV_CHANNELS; ++c)
    {
        os << HIST(c,0);
        for (size_t v = 1; v < APV_MAX_VALUE; ++v)
        {
            os << "," << HIST(c,v);
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
            os << " " << std::setw(5) << HIST(c,v);
        }
        os << "\n";
    }
}

void Pedestal::printGPscript (std::ostream &os, std::string dataFile, double bsig, double esig) const
{
    std::string baseName = dataFile.substr(0,dataFile.find_last_of("."));
    os << "# gnuplot script for plotting APV pedestal histograms\n" <<
       "normal(x, s, mu, sd) = (s/(sd*sqrt(2*pi)))*exp(-(x-mu)**2/(2*sd**2))\n" <<
       "normal2(x, s, mu1, sd1, mu2, sd2) = normal(x,s/2,mu1,sd1) + normal(x,s/2,mu2,sd2)\n" <<
       "set style data histogram\n" <<
       "set style histogram cluster gap 1\n" <<
       "set terminal svg size 1200,960 enhanced background rgb \'white\'\n" <<
       "set style fill solid 1.0\n";
    for (size_t c = 0; c < APV_CHANNELS; ++c)
    {
        // Stats over the full range
        Tally tf = tally(HISTC(c));
        double avgf = tf.sum/tf.N;
        double varf = variance(HISTC(c), tf);
        double sigmaf = std::sqrt(varf);
        // Stats over a reduced range
        Tally tr = tally(HISTC(c),1024,2048);
        double avgr = tr.sum/tr.N;
        double varr = variance(HISTC(c), tr,1024,2048);
        double sigmar = std::sqrt(varr);
        // Double gausian curve-fit
        std::cout << "Double gausian curve-fit " << c << std::endl;
        DoubleGaussFit dgf(HISTC(c), APV_MAX_VALUE, 0, tf.sum, tf.N, varf);
        os << "set xrange [" << (int)(avgf + bsig*sigmaf) << ":" << (int)(avgf + esig*sigmaf) << "]\n" <<
           "set xtics " << avgr << "," << sigmar << "\n" <<
           "set output \'" << baseName << "-" << std::setfill('0') << std::setw(3) << c << std::setfill(' ') << ".svg\'\n" <<
           "plot \"" << dataFile << "\" using " << c+2 << " linecolor rgb \"#888888\",\\\n" <<
           "\tnormal2(x," << tf.N << ","  << dgf.gauss1.mean << "," << dgf.gauss1.sigma << "," <<
                                             dgf.gauss2.mean << "," << dgf.gauss2.sigma << ") linecolor rgb \"#0000AA\",\\\n" <<
           "\tnormal(x," << tf.N << ","  << avgf << "," << sigmaf << ") linecolor rgb \"#AA0000\",\\\n" <<
           "\tnormal(x," << tr.N << ","  << avgr << "," << sigmar << ") linecolor rgb \"#00AA00\"\n";
    }
}

std::ostream& operator<< (std::ostream& os, Pedestal const& p)
{
    p.printOn(os);
    return os;
}

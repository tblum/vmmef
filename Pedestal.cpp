//
// Created by troels on 8/30/16.
//

#include <iomanip>
#include <cmath>
#include <vector>
#include <cstring>
#include <iostream>
#include <lmstruct.h>
#include <cstdlib>
#include <sstream>
#include "vmmef.h"
#include "Pedestal.h"

#define HIST(C,V) histogram[(C)*(APV_MAX_VALUE) + (V)]
#define HISTC(C) &histogram[(C)*(APV_MAX_VALUE)]

Pedestal::Pedestal ()
{
    histogram = new int[APV_CHANNELS*APV_MAX_VALUE];
    memset(histogram,0,APV_CHANNELS*(APV_MAX_VALUE)*sizeof(int));
    std::srand(std::time(0));
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

static void cleverName(const std::vector<GaussParam>& params, const std::string& prefix, std::ostream &os)
{
    std::ostringstream sum;
    sum << prefix << "sum(x) = ";
    for (size_t g = 0; g < params.size(); ++g)
    {
        const GaussParam &p = params[g];
        os << prefix << g << "(x) = normal(x," << p.mean << "," << p.sigma << "," << p.ig << ")\n";
        if (g > 0)
        {
            sum << "+ ";
        }
        sum << prefix << g << "(x)";
    }
    os << sum.str() << "\n";
}

void Pedestal::printGPscript (std::ostream &os, std::string dataFile, int id, size_t curves,
                              bool freeIntegral, bool fixedIntegral) const
{
    std::string baseName = dataFile.substr(0,dataFile.find_last_of("."));
    os << "# gnuplot script for plotting APV pedestal histograms\n" <<
       "normal(x, mu, sd, ig) = (ig/(sd*sqrt(2*pi)))*exp(-(x-mu)**2/(2*sd**2))\n" <<
       "set style data histogram\n" <<
       "#set logscale y\n" <<
       "#set yrange [1:]\n" <<
       "set xlabel \"Signal intensity\"\n" <<
       "set ylabel \"Occurrences\"\n" <<
       "set style histogram cluster gap 1\n" <<
       "set terminal svg enhanced size 1200,960 enhanced background rgb \'white\' font \"Helvetica,20\"\n" <<
       "set style fill solid 1.0\n";
    for (size_t c = 0; c < APV_CHANNELS; ++c)
    {
        int* hist = HISTC(c);
        // Stats over the full range
        Tally tf = tally(hist);
        // Find "interesting" range
        std::pair<size_t ,size_t > range = findCutoff(hist, (size_t)tf.sum/tf.N, 10, 3);
        size_t begin = range.first;
        size_t end = range.second;
        Tally t = tally(hist,begin,end);
        double var = variance(hist, t, begin, end);
        if (verbose)
        {
            std::cout << "Gausian curve-fit APV " << id << ":" << std::setfill('0') << std::setw(3) << c <<
                      std::setfill(' ') << " data points:" << end-begin << std::endl;
        }
        os << "set xrange [" << begin << ":" << end << "]\n" <<
           "unset label\n" <<
           "set output \'" << baseName << "-" << std::setfill('0') << std::setw(3) << c << std::setfill(' ') << ".svg\'\n";

        if (fixedIntegral)
        {
            lm_status_struct lm_status;
            std::vector<GaussParam> params = gaussFit(hist + begin, end - begin, begin, t.sum, t.N, var, curves,
                                                      &lm_status, false);
            os << "# fixed norm: " << lm_status.fnorm << ", itr: " << lm_status.nfev << "\n";
            cleverName(params, "fixed", os);
            if (verbose)
            {
                std::cout << "    fixed " << lm_infmsg[lm_status.outcome] << std::endl;
            }
        }
        if (freeIntegral)
        {
            lm_status_struct lm_status;
            std::vector<GaussParam> params = gaussFit(hist + begin, end - begin, begin, t.sum, t.N, var, curves,
                                                      &lm_status, true);
            os << "# free norm: " << lm_status.fnorm << ", itr: " << lm_status.nfev << "\n";
            cleverName(params, "free", os);
            if (verbose)
            {
                std::cout << "    free  " << lm_infmsg[lm_status.outcome] << std::endl;
            }
        }

        os << "set multiplot layout 2, 1 title \"AVP" << id << " Channel " << c << "\"\n" <<
           "unset logscale y\nset yrange [0:]\n" <<
           "plot \"" << dataFile << "\" using " << c+2 << " linecolor rgb \"#888888\" title \"data\"";
        for (size_t ng = 0; ng < curves; ++ng)
        {
            if (fixedIntegral)
            {
                os << ",\\\n     fixed" << ng << "(x) linecolor rgb \"#FF8888\"";
            }
            if (freeIntegral)
            {
                os << ",\\\n     free" << ng << "(x) linecolor rgb \"#8888FF\"";
            }
        }
        if (fixedIntegral)
        {
            os << ",\\\n     fixedsum(x) linecolor rgb \"#FF0000\"";
        }
        if (freeIntegral)
        {
            os << ",\\\n     freesum(x) linecolor rgb \"#0000FF\"";
        }
        os << "\n" <<
           "set logscale y\nset yrange [1:]\n" <<
           "plot \"" << dataFile << "\" using " << c+2 << " linecolor rgb \"#888888\" title \"data\"";
        for (size_t ng = 0; ng < curves; ++ng)
        {
            if (fixedIntegral)
            {
                os << ",\\\n     fixed" << ng << "(x) linecolor rgb \"#FF8888\"";
            }
            if (freeIntegral)
            {
                os << ",\\\n     free" << ng << "(x) linecolor rgb \"#8888FF\"";
            }
        }
        if (fixedIntegral)
        {
            os << ",\\\n     fixedsum(x) linecolor rgb \"#FF0000\"";
        }
        if (freeIntegral)
        {
            os << ",\\\n     freesum(x) linecolor rgb \"#0000FF\"";
        }
        os << "\nunset multiplot\n";


}
}

std::ostream& operator<< (std::ostream& os, Pedestal const& p)
{
    p.printOn(os);
    return os;
}

std::pair<size_t, size_t> Pedestal::findCutoff (const int *hist, size_t o, size_t n, int v) const
{
    size_t start = o+1;
    size_t ns = 0;
    while (start > 0 && ns < n)
    {
        if (hist[--start] < v)
            ++ns;
        else
            ns = 0;
    }
    size_t end = o-1;
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

static inline double rand(double from, double to)
{
    double r = (double)(std::rand()) / (double)RAND_MAX; // random number [0:1[
    return (r*(to-from)+from);
}

std::vector<GaussParam> Pedestal::gaussFit( const int* hist // histogram data
        , size_t hist_size // number of bins
        , long x0          // offset for x
        , double sum       // sum of hist
        , long N           // number of data points
        , double variance  //
        , size_t ng        // number of gauss functions to sum for the curve fit
        , lm_status_struct* status = NULL
        , bool freeIntegral = false
) const
{
    lmng_data_t data = {ng, N, x0, hist};
    std::vector<double> par;
    double mean = sum/N;
    double sd = std::sqrt(variance);
    double ig = (double)N/(double)ng;
    for (size_t i = 0; i < ng; ++i)
    {
        par.push_back(mean+ng*sd*rand(-1.0,1.0));
        par.push_back(sd*ng*rand(0.5,2.0));
        if (freeIntegral)
        {
            par.push_back(ig/ng);
        }
    }
    lm_control_struct control = lm_control_double;
    lm_status_struct _status;
    if (status == NULL)
    {
        status = &_status;
    }
    lmmin(par.size(), par.data(), hist_size, (const void *) &data, (freeIntegral?&lmngi_eval:&lmng_eval), &control, status);
    std::vector<GaussParam> res;
    for (size_t i = 0; i < ng; ++i)
    {
        size_t pi = i*(freeIntegral?NGI_PAR:NG_PAR);
        res.push_back({par[pi],par[pi+1],(freeIntegral?par[pi+2]:ig)});
    }
    return res;
}

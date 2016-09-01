//
// Created by troels on 8/30/16.
//

#ifndef VMMEF_PEDISTAL_H
#define VMMEF_PEDISTAL_H

#include "FECEvent.h"

class Pedestal
{
private:
    int dataPoints;
    int hist[APV_CHANNELS][APV_MAX_VALUE];
    double sum[APV_CHANNELS];
    uint16_t min[APV_CHANNELS];
    uint16_t max[APV_CHANNELS];
protected:
    void printOn(std::ostream& os) const;
public:
    void printStats(std::ostream &os) const;
    void printGPdata(std::ostream &os) const;
    void printGPscript(std::ostream &os, std::string dataFile, double nsig) const;
    Pedestal();
    void add(ADCiterator& begin);
    friend std::ostream& operator<< (std::ostream& os, Pedestal const& p);
};


#endif //VMMEF_PEDISTAL_H

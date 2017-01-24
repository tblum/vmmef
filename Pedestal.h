//
// Created by troels on 8/30/16.
//

#ifndef VMMEF_PEDISTAL_H
#define VMMEF_PEDISTAL_H

#include "vmmef.h"
#include "Histogram.h"
#include "HDF5File.h"

// number of strips in each dimension
#define STRIPS (DAQ_CHANNELS/2)*APV_CHANNELS

class Pedestal
{
private:
    uint16_t avg[2][STRIPS]; // Average noise level
    uint16_t cut[2][STRIPS]; // Cutoff noise level
public:
    Pedestal(const Histogram& histogram);
    uint16_t cutOff(size_t img, size_t column) const {return cut[img][column];}
    friend void HDF5File::addPedestal(Pedestal* pedestal, Histogram* histogram);
};


#endif //VMMEF_PEDISTAL_H

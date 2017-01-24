//
// Created by troels on 7/7/16.
//

#ifndef VMMEF_FECEVENT_H
#define VMMEF_FECEVENT_H

#include <cstdint>
#include <string>
#include <array>
#include <H5Cpp.h>
#include "vmmef.h"
#include "ADCiterator.h"

class Histogram;
class EventImage;
class FECEvent
{
private:
    const uint32_t *const header;
    const uint32_t *const data;
    const size_t size;
    const uint32_t id;
    const std::vector<size_t >& daqMap;

    ADCiterator findData (const ADCiterator &begin, const ADCiterator &end) const;

    void writeToImage (const ADCiterator &begin, const ADCiterator &end,
                       size_t offset, uint16_t image[IMG_HEIGHT][IMG_WIDTH]) const;

public:
    FECEvent (char data[], size_t eventSize, const std::vector<size_t >& daqMap);
    EventImage generateImage () const;
    void addToHistogram (Histogram *histogram) const;
};

#endif //VMMEF_FECEVENT_H

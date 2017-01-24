//
// Created by troels on 7/7/16.
//

#include <cstddef>
#include <assert.h>
#include <iostream>
#include <vector>
#include "FECEvent.h"
#include "Histogram.h"
#include "EventImage.h"

// APV Channel order mapping function
inline size_t apvChannelNo (size_t n) {return 32*(n%4)+8*(n/4)-31*(n/16);}
// APV digital header value
inline bool apvOne(uint16_t v) {return v<1200;}
inline bool apvZero(uint16_t v) {return v>3000;}


FECEvent::FECEvent(char _data[], size_t eventSize, const std::vector<size_t >& _daqMap)
        : header((uint32_t*)_data)
        , data(header+7)
        , size(eventSize)
        , id(header[2])
        , daqMap(_daqMap)

{
    assert(size == header[0]);
}

// Return iterator pointing to the first true data entry
ADCiterator FECEvent::findData(const ADCiterator& begin, const ADCiterator& end) const
{
    int header = 0;
    ADCiterator it(begin);
    for (; it != end; ++it) // First we look for the header '111'
    {
        if (((apvOne(*it)) ? ++header : header = 0) >= 3)
        {
            it += APV_ADDRESS_BITS+2; //incr + skip 8 bit address + error bit
            assert(apvOne(*(it-1))); // Error bit should be 1
            break; //We found the header
        }
    }
    assert(it != end); //Make sure we found the header
    return it;
}

void FECEvent::writeToImage(const ADCiterator& begin, const ADCiterator& end,
                            size_t offset, uint16_t image[IMG_HEIGHT][IMG_WIDTH]) const
{
    ADCiterator it = findData(begin, end);
    assert(it + (IMG_HEIGHT*(APV_CHANNELS+APV_HEADER_BITS) - APV_HEADER_BITS) < end); //Make sure we don't overrun
    for (size_t i = 0; i < IMG_HEIGHT; ++i)
    {
        for (size_t j = 0; j < APV_CHANNELS; ++j)
        {
            image[i][apvChannelNo(j)+offset] = APV_MAX_VALUE - (*it++);
        }
        it += APV_HEADER_BITS;
    }
}

inline size_t apvSize(const u_int32_t* data, unsigned char channel)
{
    uint32_t adc = data[1];
    assert (adc == (u_int32_t)(0x41444300+channel)); // 'ADC_'+#C
    size_t size = *((uint16_t*)(data+2));
    assert (size == 4000);
    return size;
}

// Mapping between DAQ & Channel <-> Image & Offset/Column
inline size_t imgNo(size_t daq) {return daq < 2 ? 0 : 1;}
inline size_t imgOffset(size_t daq) {return daq%2==0?0:APV_CHANNELS;}

EventImage FECEvent::generateImage() const
{
    EventImage img;
    const u_int32_t* ptr = data;
    for (unsigned char daq = 0; daq < DAQ_CHANNELS; ++daq)
    {
        size_t size = apvSize(ptr, daq);
        ptr += SRS_HEADER_SIZE;
        ADCiterator begin(ptr);
        ADCiterator end = begin + size;
        writeToImage(begin, end, imgOffset(daqMap[daq]), img.data[imgNo(daqMap[daq])]);
        ptr += size/2; // since the size is in 16 bit words
    }
    return img;
}


void FECEvent::addToHistogram (Histogram* histogram) const
{
    const u_int32_t* ptr = data;
    for (unsigned char daq = 0; daq < DAQ_CHANNELS; ++daq)
    {
        size_t size = apvSize(ptr, daq);
        ptr += SRS_HEADER_SIZE;
        ADCiterator begin(ptr);
        ADCiterator end = begin + size;
        ADCiterator it = findData(begin, end);

        for (size_t tb = 0; tb < APV_TIME_BINS; ++tb)
        {
            for (size_t c = 0; c < APV_CHANNELS; ++c)
            {
                uint16_t v = (APV_MAX_VALUE - (*it++));
                histogram->inc(daqMap[daq]*APV_CHANNELS+apvChannelNo(c),v);
            }
            it += APV_HEADER_BITS;
        }

        ptr += size/2; // since the size is in 16 bit words
    }

}

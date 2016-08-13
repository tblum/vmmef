//
// Created by troels on 7/7/16.
//

#include <cstddef>
#include <assert.h>
#include <H5Cpp.h>
#include <iostream>
#include <vector>
#include "FECEvent.h"
#include "ADCiterator.h"

#define APV_ONE(x)  ((x) < 1200)
#define APV_ZERO(x) ((x) > 3000)

#define APV_ADDRESS_BITS 8
#define APV_HEADER_BITS 12
#define APV_CHANELS 128
#define DAQ_CHANELS 4
#define SRS_HEADER_SIZE 3

FECEvent::FECEvent(char _data[], size_t eventSize)
        : header((uint32_t*)_data)
        , data(header+7)
        , size(eventSize)
        , id(header[2])
        , imagesGenerated(false)
{
    assert(size == header[0]);
}

void FECEvent::WriteHDF5(std::string fileName)
{
    generateImages();
    try
    {
        H5::H5File file(fileName, H5F_ACC_TRUNC );

    }
   catch(H5::Exception e)
   {
       std::cerr << e.getDetailMsg() << std::endl;
       throw e;
   }
}


void FECEvent::writeToImage (const ADCiterator& begin, const ADCiterator& end, size_t offset, Image& image)
{
    int header = 0;
    ADCiterator it(begin);
    for (; it != end; ++it) // First we look for the header '111'
    {
        if (((APV_ONE(*it)) ? ++header : header = 0) >= 3)
        {
            it += APV_ADDRESS_BITS+2; //incr + skip 8 bit address + error bit
            assert(APV_ONE(*(it-1))); // Error bit should be 1
            break; //We found the header
        }
    }

    assert(it != end); //Make sure we found the header
    assert(it + (IMG_HEIGHT*(APV_CHANELS+APV_HEADER_BITS) - APV_HEADER_BITS) < end); //Make sure we don't overrun
    for (size_t i = 0; i < IMG_HEIGHT; ++i)
    {
        for (size_t j = offset; j < offset+APV_CHANELS; ++j)
        {
            image[j] = UINT16_MAX - *it++;
        }
        it += APV_HEADER_BITS;
    }
}

inline size_t apvSize(const u_int32_t* data, char channel)
{
    uint32_t adc = data[1];
    assert (adc == (u_int32_t)(0x41444300+channel)); // 'ADC_'+#C
    size_t size = *((uint16_t*)(data+2));
    assert (size == 4000);
    return size;
}

void FECEvent::generateImages ()
{
    if (imagesGenerated)
    {
        return;
    }
    const u_int32_t* ptr = data;
    for (char c = 0; c < DAQ_CHANELS; ++c)
    {
        size_t size = apvSize(ptr, c);
        ptr += SRS_HEADER_SIZE;
        ADCiterator begin(ptr);
        ADCiterator end = begin + size;
        writeToImage(begin, end, 0, imageX);
        assert (size == 4000);
        ptr += size/2; // since the size is in 16 bit words
    }
}

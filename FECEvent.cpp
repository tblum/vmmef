//
// Created by troels on 7/7/16.
//

#include <cstddef>
#include <assert.h>
#include <iostream>
#include <vector>
#include <string>
#include "FECEvent.h"
#include "ADCiterator.h"


FECEvent::FECEvent(char _data[], size_t eventSize)
        : header((uint32_t*)_data)
        , data(header+7)
        , size(eventSize)
        , id(header[2])
        , imagesGenerated(false)
{
    assert(size == header[0]);
}

static int eid = 0;
void FECEvent::WriteToHDF5(H5::H5File file)
{
    Images img = generateImages();
    try
    {
        const hsize_t dims[3] = {2, IMG_HEIGHT, IMG_WIDTH};
        H5::DataSpace dataspace( 3,  dims);
        H5::IntType datatype( H5::PredType::NATIVE_UINT16 );
        datatype.setOrder( H5T_ORDER_LE );
        H5::DataSet dataset = file.createDataSet(std::to_string(eid++), datatype, dataspace );
        dataset.write( img.data, H5::PredType::NATIVE_UINT16 );
    }
    catch(H5::Exception e)
    {
        std::cerr << e.getDetailMsg() << std::endl;
        throw e;
    }
}

// Return iterator pointing to the first true data entry
ADCiterator FECEvent::findData(const ADCiterator& begin, const ADCiterator& end) const
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
    return it;
}

void FECEvent::writeToImage(const ADCiterator& begin, const ADCiterator& end, size_t offset, uint16_t image[IMG_HEIGHT][IMG_WIDTH])
{
    ADCiterator it = findData(begin, end);
    assert(it + (IMG_HEIGHT*(APV_CHANNELS+APV_HEADER_BITS) - APV_HEADER_BITS) < end); //Make sure we don't overrun
    for (size_t i = 0; i < IMG_HEIGHT; ++i)
    {
        for (size_t j = offset; j < offset+APV_CHANNELS; ++j)
        {
            image[i][j] = APV_MAX_VALUE - (*it++);
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

FECEvent::Images FECEvent::generateImages()
{
    Images img;
    const u_int32_t* ptr = data;
    for (char c = 0; c < DAQ_CHANNELS; ++c)
    {
        size_t size = apvSize(ptr, c);
        ptr += SRS_HEADER_SIZE;
        ADCiterator begin(ptr);
        ADCiterator end = begin + size;
        writeToImage(begin, end, c%2==0?0:APV_CHANNELS, c<2?img.proj.x:img.proj.y);
        ptr += size/2; // since the size is in 16 bit words
    }
    return img;
}

void FECEvent::addToPedistals (Pedestal* pedestal[DAQ_CHANNELS])
{
    const u_int32_t* ptr = data;
    for (unsigned char c = 0; c < DAQ_CHANNELS; ++c)
    {
        size_t size = apvSize(ptr, c);
        ptr += SRS_HEADER_SIZE;
        ADCiterator begin(ptr);
        ADCiterator end = begin + size;
        ADCiterator it = findData(begin, end);
        pedestal[c]->add(it);
        ptr += size/2; // since the size is in 16 bit words
    }

}

//
// Created by troels on 7/7/16.
//

#ifndef VMMEF_FECEVENT_H
#define VMMEF_FECEVENT_H

#define APV_CHANNELS 128
#define APV_MAX_VALUE (1<<12)
#define APV_TIME_BINS 27
#define APV_ADDRESS_BITS 8
#define APV_HEADER_BITS 12

#define DAQ_CHANNELS 4
#define SRS_HEADER_SIZE 3

#define IMG_HEIGHT (APV_TIME_BINS)
#define IMG_WIDTH (APV_CHANNELS*2)

#define APV_ONE(x)  ((x) < 1200)
#define APV_ZERO(x) ((x) > 3000)

#include <cstdint>
#include <string>
#include <array>
#include <H5Cpp.h>

#include "ADCiterator.h"
#include "Pedestal.h"


class Pedestal;
class FECEvent
{
private:
    const uint32_t* const header;
    const uint32_t* const data;
    const uint32_t size;
    const uint32_t id;
    bool imagesGenerated;
    union Images
    {
        struct
        {
            uint16_t x[IMG_HEIGHT][IMG_WIDTH];
            uint16_t y[IMG_HEIGHT][IMG_WIDTH];
        }proj;
        uint16_t data [2][IMG_HEIGHT][IMG_WIDTH];
    };
    ADCiterator findData(const ADCiterator& begin, const ADCiterator& end) const;
    void writeToImage (const ADCiterator& begin, const ADCiterator& end, size_t offset, uint16_t image[IMG_HEIGHT][IMG_WIDTH]);
public:
    FECEvent(char data[], size_t eventSize);
    Images generateImages();
    uint32_t getID() {return id;}
    void WriteToHDF5(H5::H5File file);
    void addToPedistals(Pedestal* pedistal[DAQ_CHANNELS]);

};


#endif //VMMEF_FECEVENT_H

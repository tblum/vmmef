//
// Created by troels on 7/7/16.
//

#ifndef VMMEF_FECEVENT_H
#define VMMEF_FECEVENT_H


#include <cstdint>
#include <string>
#include <array>
#include "ADCiterator.h"

#define IMG_HEIGHT 27
#define IMG_WIDTH 256


class FECEvent
{
    typedef std::array<uint16_t,IMG_HEIGHT*IMG_WIDTH> Image;
private:
    const uint32_t* const header;
    const uint32_t* const data;
    const uint32_t size;
    const uint32_t id;
    bool imagesGenerated;
    Image imageX;
    Image imageY;
    void writeToImage (const ADCiterator &begin, const ADCiterator &end, size_t offset, Image& image);
public:
    FECEvent(char data[], size_t eventSize);
    FECEvent (const std::vector<uint16_t> &image);
    void generateImages();
    uint32_t getID() {return id;}
    void WriteHDF5(std::string fileName);


};


#endif //VMMEF_FECEVENT_H

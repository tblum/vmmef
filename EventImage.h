//
// Created by troels on 11/17/16.
//

#ifndef VMMEF_EVENTIMAGE_H
#define VMMEF_EVENTIMAGE_H
#include "FECEvent.h"
#include "Pedestal.h"

class EventImage
{
private:
    friend class FECEvent;
    uint16_t data [2][IMG_HEIGHT][IMG_WIDTH];
public:
    const uint16_t* images() {return data[0][0];}
    void applyPedestal(Pedestal* pedestal);
};


#endif //VMMEF_EVENTIMAGE_H

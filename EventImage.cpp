//
// Created by troels on 11/17/16.
//

#include "EventImage.h"

void EventImage::applyPedestal (Pedestal *pedestal)
{
    for (size_t img = 0; img < 2; ++img)
    {
        for (size_t h = 0; h < IMG_HEIGHT; ++h)
        {
            for (size_t w = 0; w < IMG_WIDTH; ++w)
            {
                uint16_t p = data[img][h][w];
                uint16_t cut = pedestal->cutOff(img,w);
                data[img][h][w] = p < cut ? 0 : p - cut;
            }
        }
    }
}

//
// Created by troels on 8/30/16.
//

#include <iostream>
#include <iomanip>
#include "Pedestal.h"

Pedestal::Pedestal (const Histogram &histogram)
{
    for (size_t img = 0; img < 2; ++img)
    {
        for (size_t strip = 0; strip < STRIPS; ++strip)
        {
            const size_t hs = img*STRIPS+strip; // histogram strip_id
            uint16_t average = histogram.average(hs);
            avg[img][strip] = average;
            cut[img][strip] = histogram.findCutoff(hs, average, 10, 2);
        }
    }
}

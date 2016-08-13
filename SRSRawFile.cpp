//
// Created by troels on 6/30/16.
//

#include <iostream>
#include "SRSRawFile.h"

#define BUF_SIZE 1024*1024

SRSRawFile::SRSRawFile(std::string fileName)
{
    file.exceptions(std::ifstream::failbit | std::ifstream::badbit | std::ifstream::eofbit);
    file.open(fileName, std::ios::in|std::ios::binary);
}

FECEvent SRSRawFile::getNextFECEvent()
{
    eventHeaderStruct eventHeader;
    while (true)
    {
        file.read((char*)&eventHeader,sizeof(eventHeader));
        if (eventHeader.eventType == PHYSICS_EVENT)
            break;
    }
    size_t eventSize = eventHeader.eventSize - sizeof(eventHeader);
    char event[eventSize*sizeof(u_int32_t)];
    file.read(event,eventSize);
    return FECEvent(event,eventSize);
}




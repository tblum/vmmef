//
// Created by troels on 6/30/16.
//

#include <iostream>
#include "SRSRawFile.h"

SRSRawFile::SRSRawFile(std::string fileName, const std::vector<size_t >& _daqMap)
        : buffer(NULL)
        , bufferSize(0)
        , daqMap(_daqMap)
        , events(0)
{
    file.exceptions(std::ifstream::failbit | std::ifstream::badbit | std::ifstream::eofbit);
    file.open(fileName, std::ios::in|std::ios::binary);
}

SRSRawFile::~SRSRawFile ()
{
    delete[] buffer;
    if (verbose)
    {
        std::cout << "SRSRawFile: read " << events << " events." << std::endl;
    }
}

FECEvent SRSRawFile::nextFECEvent ()
{
    eventHeaderStruct eventHeader;
    while (true)
    {
        file.read((char*)&eventHeader,sizeof(eventHeader));
        if (eventHeader.eventType == PHYSICS_EVENT)
            break;
    }
    size_t eventSize = eventHeader.eventSize - sizeof(eventHeader);
    if (buffer == NULL || bufferSize < eventSize*sizeof(u_int32_t))
    {
        delete[] buffer;
        bufferSize = eventSize * sizeof(u_int32_t);
        buffer = new char[bufferSize];
    }

    file.read(buffer,eventSize);
    ++events;
    return FECEvent(buffer,eventSize, daqMap);
}

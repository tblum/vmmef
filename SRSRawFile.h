//
// Created by troels on 6/30/16.
//

#ifndef VMMEF_SRSRAWFILE_H
#define VMMEF_SRSRAWFILE_H


#include <string>
#include <fstream>
#include <vector>
#include "event.h"
#include "FECEvent.h"

class SRSRawFile
{
private:
    std::ifstream file;
public:
    SRSRawFile(std::string fileName);
    FECEvent getNextFECEvent();
};


#endif //VMMEF_SRSRAWFILE_H

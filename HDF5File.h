//
// Created by troels on 10/27/16.
//

#ifndef VMMEF_HDF5FILE_H
#define VMMEF_HDF5FILE_H

#include <string>
#include <H5Cpp.h>
#include "FECEvent.h"

class Pedestal;
class SRSRawFile;
class HDF5File
{
private:
    H5::H5File* file;
    Pedestal* pedestal;
    uint events;
    H5::Group imagegroup;
public:
    HDF5File(std::string fileName);
    void addPedestal(Pedestal* pedestal, Histogram *histogram=NULL);
    void addEvent(const FECEvent& event);
    void addEvents(SRSRawFile& file, int n);
    ~HDF5File ();
};

#endif //VMMEF_HDF5FILE_H

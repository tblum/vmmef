#include <iostream>
#include <tclap/CmdLine.h>
#include "SRSRawFile.h"
#include "vmmef.h"
#include "event.h"
#include "Pedestal.h"
#include "HDF5File.h"

int verbose = 0; // global verbosity level

void buildDAQMap(std::string str, std::vector<size_t >& daqMap) {
    std::stringstream ss(str); // Turn the string into a stream.
    std::string idx;
    while(std::getline(ss, idx, ','))
    {
        size_t i = std::stoi(idx);
        daqMap.push_back(i);
    }
}

int main(int argc, char** argv)
{
    // A few safety measures against DATE's event.h
    assert(sizeof(uint32_t) == sizeof(long32));
    assert(sizeof(eventHeaderStruct) == EVENT_HEAD_BASE_SIZE);
    Histogram* histogram = NULL;
    Pedestal* pedestal = NULL;
    HDF5File* hdf5File = NULL;
    std::vector<size_t > daqMap;

    try
    {
        // Setting up the command line perser
        TCLAP::CmdLine cmd("Event formation proto type for the VMM3", ' ', "0.001");
        TCLAP::ValueArg<std::string> dataFile_("d","data","Name of raw data file",false,"","filename",cmd);
        TCLAP::ValueArg<int> numEvents_("n","nevent","Number of events to read from file",false,-1,"int",cmd);
        TCLAP::ValueArg<int> numPedEvents_("N","nped","Number of events to read from pedestal file",false,-1,"int",cmd);
        TCLAP::ValueArg<std::string> pedFile_("p","pedestal","Name of file containing pedestal data", false,"","filename",cmd);
        TCLAP::ValueArg<std::string> aggFile_("a","aggregate","Aggregate the data in file in the same manner as the pedestal", false,"","filename",cmd);
        TCLAP::ValueArg<std::string> hdf5File_("o","hdf5","Write output to HDF5 file <filename>", false,"","filename",cmd);
        TCLAP::ValueArg<std::string> x_("x","Xmap","Comma separated list of DAQ indexes mapping to the X projection", false,"1,0","int,int",cmd);
        TCLAP::ValueArg<std::string> y_("y","Ymap","Comma separated list of DAQ indexes mapping to the Y projection", false,"3,2","int,int",cmd);
        TCLAP::ValueArg<int> verbose_("v","verbose","Level of verbosity [0-1]",false,0,"int",cmd);

        // Parsing commandline paramneters
        cmd.parse(argc, argv);
        std::string dataFileName = dataFile_.getValue();
        std::string pedestalFileName = pedFile_.getValue();
        std::string aggregateFileName = aggFile_.getValue();
        std::string hdf5FileName = hdf5File_.getValue();
        int numEvents = numEvents_.getValue();
        int numPedEvents = numPedEvents_.getValue();
        verbose = verbose_.getValue();
        buildDAQMap(x_.getValue(),daqMap);
        if (daqMap.size() != DAQ_CHANNELS/2)
        {
            std::cerr << "Xmap must consist of " << DAQ_CHANNELS/2 << " elements." << std::endl;
            return 1;
        }
        buildDAQMap(y_.getValue(),daqMap);
        if (daqMap.size() != DAQ_CHANNELS)
        {
            std::cerr << "Ymap must consist of " << DAQ_CHANNELS/2 << " elements." << std::endl;
            return 1;
        }

        if (!hdf5FileName.empty())
        {
            hdf5File = new HDF5File(hdf5FileName);
        }

        // Start processing
        if (!pedestalFileName.empty())
        {
            SRSRawFile pedFile(pedestalFileName,daqMap);
            histogram = new Histogram(pedFile, numPedEvents);
            pedestal = new Pedestal(*histogram);
            if (hdf5File)
                hdf5File->addAggregate(pedestal,histogram, "/pedestal");
        }

        if (!aggregateFileName.empty())
        {
            SRSRawFile aggFile(aggregateFileName,daqMap);
            histogram = new Histogram(aggFile, numPedEvents);
            pedestal = new Pedestal(*histogram);
            if (hdf5File)
                hdf5File->addAggregate(pedestal,histogram,"/aggregate");
        }

        if (!dataFileName.empty())
        {
            SRSRawFile dataFile(dataFileName,daqMap);
            if (hdf5File)
                hdf5File->addEvents(dataFile,numEvents);
        }

    }
    catch (TCLAP::ArgException &e)
    {
        std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl;
    }
    catch (std::exception e)
    {
        std::cerr << "error: " << e.what() << std::endl;
    }

    // Clean up
    if (pedestal)
        delete pedestal;
    if (histogram)
        delete histogram;
    if (hdf5File)
        delete hdf5File;

    return 0;
}
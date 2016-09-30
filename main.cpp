#include <iostream>
#include <tclap/CmdLine.h>
#include "SRSRawFile.h"
#include "vmmef.h"
#include "event.h"
#include "Pedestal.h"

int verbose = 0; // global verbosity level

int main(int argc, char** argv)
{
    // A few safety measures against DATE's event.h
    assert(sizeof(uint32_t) == sizeof(long32));
    assert(sizeof(eventHeaderStruct) == EVENT_HEAD_BASE_SIZE);

    try
    {
        TCLAP::CmdLine cmd("Event formation proto type for the VMM3", ' ', "0.001");
        TCLAP::ValueArg<std::string> fileName_("f","file","Name of raw data file",true,"stdin","filename",cmd);
        TCLAP::ValueArg<int> numEvents_("n","nevent","Number of events to read from tile",false,-1,"int",cmd);
        TCLAP::SwitchArg pedistal_("p","pedistal","Calculate pedistal", cmd, false);
        TCLAP::ValueArg<std::string> gnuPlot_("g","gnuplot","Output GNU-plot histogram data",false,"histogram","basename",cmd);
        TCLAP::ValueArg<size_t > curves_("b","bell","Number of Gauss/Bell curves to fit",false,3,"int",cmd);
        TCLAP::ValueArg<int> freeIntegral_("i","free_integral","Integral is a free variable when curve fitting [0=no,1=yes,>1=both]",false,0,"int",cmd);
        TCLAP::ValueArg<int> verbose_("v","verbose","Level of verbosity [0-1]",false,0,"int",cmd);
        cmd.parse(argc, argv);
        std::string fileName = fileName_.getValue();
        bool pedistal = pedistal_.getValue();
        int numEvents = numEvents_.getValue();
        size_t curves = curves_.getValue();
        int fi = freeIntegral_.getValue();
        bool freeIntegral = fi>0;
        bool fixedIntegral = (fi==0||fi>1);
        verbose = verbose_.getValue();
        bool gnuplot = false;
        if (gnuPlot_.isSet())
        {
            gnuplot = true;
        }
        SRSRawFile file(fileName);
        H5::H5File H5file("result.h5", H5F_ACC_TRUNC );
        Pedestal *pedestals[DAQ_CHANNELS];
        if (pedistal)
        {
            for (unsigned char c = 0; c < DAQ_CHANNELS; ++c)
            {
                pedestals[c] = new Pedestal();
            }
        }
        int i = 0;
        while(++i)
        {
            try
            {
                FECEvent event = file.getNextFECEvent();
                if (pedistal)
                {
                    event.addToPedistals(pedestals);
                } else {
                    event.WriteToHDF5(H5file);
                }
            }
            catch (const std::exception& e) // TODO only catch EOF
            {
                std::cout << "EOF: " << e.what() << std::endl;
                break;
            }
            if (numEvents > 0 && i > numEvents)
                break;
        }
        if (gnuplot)
        {
            std::string basename = gnuPlot_.getValue();
            std::ofstream gpFile;
            gpFile.open(basename + ".gp");
            for (int c = 0; c < DAQ_CHANNELS; ++c)
            {
                std::ofstream dataFile;
                std::string dataFileName = basename + std::to_string(c) + ".data";
                dataFile.open (dataFileName);
                dataFile << "# Pedestal DAQ" << (int)c << std::endl;
                pedestals[c]->printGPdata(dataFile);
                dataFile << std::endl;
                dataFile.close();
                pedestals[c]->printGPscript(gpFile,dataFileName, c, curves, freeIntegral, fixedIntegral);
            }
            gpFile << std::endl;
            gpFile.close();
        }
        if (pedistal)
        {
            for (unsigned char c = 0; c < DAQ_CHANNELS; ++c)
            {
                delete pedestals[c];
            }
        }
        std::cout << "Processed " << i << " events." << std::endl;

    }
    catch (TCLAP::ArgException &e)
    {
        std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl;
    }
    catch (std::exception e)
    {
        std::cerr << "error: " << e.what() << std::endl;
    }
    return 0;
}
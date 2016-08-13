#include <iostream>
#include <tclap/CmdLine.h>
#include "SRSRawFile.h"
#include "event.h"

int main(int argc, char** argv)
{
    // A few safety measures against DATE's event.h
    assert(sizeof(uint32_t) == sizeof(long32));
    assert(sizeof(eventHeaderStruct) == EVENT_HEAD_BASE_SIZE);

    try
    {
        TCLAP::CmdLine cmd("Event formation proto type for the VMM3", ' ', "0.001");
        TCLAP::ValueArg<std::string> fileName_("f","file","Name of raw data file",true,"stdin","string");
        cmd.add(fileName_);
        cmd.parse(argc, argv);
        std::string fileName = fileName_.getValue();
        SRSRawFile file(fileName);
        int i = 0;
        while(true)
        {
            try
            {
                FECEvent event = file.getNextFECEvent();
                std::cout << "FEC ID:" << event.getID() << std::endl;
                event.generateImages();
                std::cout << std::endl;
            }
            catch (const std::exception& e) // TODO only catch EOF
            {
                std::cout << "EOF: " << e.what() << std::endl;
                return 0;
            }
            if (++i > 0)
                break;
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

    return 0;
}
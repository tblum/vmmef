//
// Created by troels on 10/27/16.
//

#include <iostream>
#include <sstream>
#include <iomanip>
#include "HDF5File.h"
#include "EventImage.h"
#include "SRSRawFile.h"
#include "Pedestal.h"


HDF5File::HDF5File (std::string fileName)
        : pedestal(NULL)
        , events(0)
{
    file = new H5::H5File(fileName, H5F_ACC_TRUNC);
    imagegroup = file->createGroup("/images");

}

HDF5File::~HDF5File ()
{
    file->close();
}

void HDF5File::addPedestal (Pedestal *pedestal_, Histogram *histogram)
{
    pedestal = pedestal_;
    try
    {
        H5::Group pedgroup(file->createGroup( "/pedestal"));
        if (histogram)
        {
            const hsize_t dims[2] = {DAQ_CHANNELS*APV_CHANNELS, APV_MAX_VALUE};
            H5::DataSpace dataspace( 2,  dims);
            H5::IntType datatype( H5::PredType::NATIVE_UINT );
            datatype.setOrder( H5T_ORDER_LE );
            H5::DataSet dataset = pedgroup.createDataSet("histogram", datatype, dataspace );
            dataset.write(histogram->data, H5::PredType::NATIVE_UINT);
        }
        const hsize_t dims[2] = {2, IMG_WIDTH};
        H5::DataSpace dataspace( 2,  dims);
        H5::IntType datatype( H5::PredType::NATIVE_UINT16 );
        datatype.setOrder( H5T_ORDER_LE );
        H5::DataSet dataset_avg = pedgroup.createDataSet("average", datatype, dataspace );
        dataset_avg.write(pedestal->avg, H5::PredType::NATIVE_UINT16);
        H5::DataSet dataset_cut = pedgroup.createDataSet("cut-off", datatype, dataspace );
        dataset_cut.write(pedestal->cut, H5::PredType::NATIVE_UINT16);

    }
    catch(H5::Exception e)
    {
        std::cerr << e.getDetailMsg() << std::endl;
        throw e;
    }

}

void HDF5File::addEvent(const FECEvent& event)
{
    EventImage image = event.generateImage();
    //if (pedestal)
    //    image.applyPedestal(pedestal);
    std::stringstream evs;
    evs << std::setw(7) << std::setfill('0') << events++;
    try
    {
        const hsize_t dims[3] = {2, IMG_HEIGHT, IMG_WIDTH};
        H5::DataSpace dataspace( 3,  dims);
        H5::IntType datatype( H5::PredType::NATIVE_UINT16 );
        datatype.setOrder( H5T_ORDER_LE );

        H5::DataSet dataset = imagegroup.createDataSet(evs.str(), datatype, dataspace );
        dataset.write(image.images(), H5::PredType::NATIVE_UINT16 );
    }
    catch(H5::Exception e)
    {
        std::cerr << e.getDetailMsg() << std::endl;
        throw e;
    }

}

void HDF5File::addEvents (SRSRawFile &file, int n)
{
    int i = 0;
    while(++i)
    {
        try
        {
            const FECEvent event = file.nextFECEvent();
            addEvent(event);
        }
        catch (const std::exception& e) // TODO only catch EOF
        {
            break;
        }
        if (n > 0 && i > n)
            break;
    }

}

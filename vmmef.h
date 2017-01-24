//
// Created by troels on 9/21/16.
//

#ifndef VMMEF_VMMEF_H
#define VMMEF_VMMEF_H
#include <cstddef>
#include <vector>

typedef unsigned int uint;
extern int verbose;

/*
 * Constants defines by hardware and layout
 */
#define APV_CHANNELS 128
#define APV_MAX_VALUE (uint16_t)(1<<12)
#define APV_TIME_BINS 27
#define APV_ADDRESS_BITS 8
#define APV_HEADER_BITS 12

#define DAQ_CHANNELS 4
#define SRS_HEADER_SIZE 3

#define IMG_HEIGHT (APV_TIME_BINS)
#define IMG_WIDTH (APV_CHANNELS*2)

#endif //VMMEF_VMMEF_H

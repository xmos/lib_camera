// Copyright 2023 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#pragma once

/**
 * The `<xcore/>` headers cannot be safely included from XC files, but 
 * several of the types defined in those headers need to be available in XC. 
 * This header is a work-around which defines those types for XC but includes
 * them for C or CPP files.
 */

#ifdef __XC__
    typedef chanend chanend_t;          // In order to use chanend_t in XC
#else //__XC__
    #include <xcore/channel.h>          // include channel, channend, streaming channel
    #include <xccompat.h>               // for ports, clocks, and channends in purely C
    typedef chanend chanend_t;          // chanend_t in C imported from XC
#endif //__XC__

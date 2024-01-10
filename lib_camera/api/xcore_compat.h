// Copyright 2023-2024 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#pragma once

/**
 * The `<xcore/>` headers cannot be safely included from XC files, but 
 * several of the types defined in those headers need to be available in XC. 
 * This header is a work-around which defines those types for XC but includes
 * them for C or CPP files.
 */

#ifdef __XC__
    typedef chanend chanend_t;          // chanend_t (XC only)
#else //__XC__
    #include <xcore/channel.h>          // include channel, channend, streaming channel (C only)
    #include <xccompat.h>               // include ports, clocks, and channends (XC/C compat)
    typedef chanend chanend_t;          // chanend_t (XC/C compat)
#endif //__XC__

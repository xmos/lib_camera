#include "mipi.h"

#ifdef __XC__
extern "C" {
#endif


void MipiPacketRx_simulate(
    in_buffered_port_32_t p_mipi_rxd,
    in_port_t p_mipi_rxa,
    streaming_chanend_t c_pkt,
    streaming_chanend_t c_ctrl);


#ifdef __XC__
}
#endif

#ifndef _RSTPT_H_
#define _RSTPT_H_


void 
CREATE_RSTP_INSTANCE();
u16_t 
GET_STATUS();
void 
GET_BPDU();
void
CHECK_on_down(u16_t The_XOR,u16_t The_current);
void
SIMULATE_TCN(int port_index);
void 
SIMULATE_RSTBPDU(int port_index);
void 
SIMULATE_CONFIGURE(int port_index);

#endif
/********************************************************************
 * Copyright (C) 2012 by ##LAB_REMOVED_FOR_BLIND_REVIEW##                                    *
 * Initial author: ##AUTHOR_REMOVED_FOR_BLINF_REVIEW##                                     *
 ********************************************************************/

/*!
  \file uart.h
  \brief UART module


*/
#ifndef UART_H
#define UART_H

#include "ensitlm.h"

class UART : public sc_core::sc_module {
public:
	ensitlm::target_socket<UART> target;

	tlm::tlm_response_status read(ensitlm::addr_t a, ensitlm::data_t &d);

	tlm::tlm_response_status write(ensitlm::addr_t a, ensitlm::data_t d);

	SC_CTOR(UART){/* */};

private:
};

#endif // UART_H

/*
 * 
 * This file is part of the Contiki operating system.
 *
 */

/**
 * \file
 *         Coordinator Source Code for NES project
 * \author
 *         Joakim Eriksson, joakime@sics.se
 *          Adam Dunkels, adam@sics.se
 * 			Yuefeng Wu, y.wu.5@student.tue.nl
 */

#include "contiki.h"
#include "net/rime/rime.h"
#include "sys/etimer.h"
#include "dev/leds.h"
#include "net/netstack.h"
#include "cc2420.h"
#include "cc2420_const.h"
#include <stdlib.h>
#include "dev/spi.h"
#include <stdio.h>
#include <string.h>


/*---------------------------------------------------------------------------*/
/* This is a set of predefined values for this laboratory*/

#define WITH_SEND_CCA 0

/*---------------------------------------------------------------------------*/
/* This is a set of global variables for this laboratory*/

int currentChannel = 26;


/*---------------------------------------------------------------------------*/
PROCESS(channel_interferencer, "Channel Interferencer");
AUTOSTART_PROCESSES(&channel_interferencer);


/*---------------------------------------------------------------------------*/
static void
broadcast_recv(struct broadcast_conn *c, const linkaddr_t *from)
{
    printf("broadcast message received from %d.%d: '%s'\n",
           from->u8[0], from->u8[1], (char *)packetbuf_dataptr());
}
static const struct broadcast_callbacks broadcast_call = {broadcast_recv};
static struct broadcast_conn broadcast;
/*---------------------------------------------------------------------------*/


PROCESS_THREAD(channel_interferencer, ev, data)
{
	
        PROCESS_EXITHANDLER(broadcast_close(&broadcast);)
        
        PROCESS_BEGIN();
        
        broadcast_open(&broadcast, 129, &broadcast_call);
	
	/* switch mac layer off, and turn radio on */

	
	NETSTACK_MAC.off(0);
	
        cc2420_on();
        
        cc2420_set_txpower(1);
        
        
        cc2420_set_channel(currentChannel);
        
        
        
	while(1) {
            
            packetbuf_copyfrom("H", 1);
            broadcast_send(&broadcast);
	}

	PROCESS_END();
}
/*---------------------------------------------------------------------------*/

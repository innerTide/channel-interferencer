/*
 * 
 * This file is part of the Contiki operating system.
 *
 */

/**
 * \file
 *         Interference Generator Source Code for NES project
 * \author
 *         Yuefeng Wu, y.wu.5@student.tue.nl
 */

#include "contiki.h"
#include "net/rime/rime.h"
#include "dev/leds.h"
#include "sys/etimer.h"
#include "net/netstack.h"
#include "cc2420.h"
#include "cc2420_const.h"
#include <stdlib.h>
#include "dev/spi.h"
#include <stdio.h>
#include <string.h>
#include "sys/node-id.h"
#include <stdlib.h>


/*---------------------------------------------------------------------------*/
/* This is a set of predefined values for this laboratory*/

// #define WITH_SEND_CCA 0 /*Turning CCA off, since a jammer does not need CCA!*/
#define INTERFERENCE_SCENARIO 1 /*Interference scenario setting: 
                                  0 for Wi-Fi, 
                                  1 for linear increment,
                                  2 for randomly generated energy
                                  3 for specified interference*/
#define NODE_ID_ALIGNMENT 0  //This value should be the node ID of interferencer working in CH-11 MINUS 1
#define CONSTANT_TX_POWER 31 //This is the TX power for specified interference (3)
#define SPECIFIED_CHANNEL 26 //This is the Channel for specified interference (3)


/*---------------------------------------------------------------------------*/
/* This is a function to generate random TX power table for this laboratory*/
static void random_table_generator (char* txPowerTable){
    unsigned char i;
    for (i = 0; i < 16; i++ ){
        *txPowerTable = 9 + rand()%23; //generate a number in [9,31]
        txPowerTable++;
    }
}


/*---------------------------------------------------------------------------*/
PROCESS(channel_interferencer, "Channel Interferencer");
AUTOSTART_PROCESSES(&channel_interferencer);


/*---------------------------------------------------------------------------*/
static struct broadcast_conn broadcast;
/*---------------------------------------------------------------------------*/


PROCESS_THREAD(channel_interferencer, ev, data)
{
	
        PROCESS_EXITHANDLER(broadcast_close(&broadcast);)
        
        PROCESS_BEGIN();
        
        static struct etimer jamTimer;
        
        
        /*---------------------------------------------------------------------------*/
        /* This is a set of interference scenarios for this laboratory*/
        
        #if INTERFERENCE_SCENARIO==0
        unsigned char txPowerTable [16] = {22,30,25,17,10,16,28,25,16,10,22,31,26,18,10,6};
        #elif INTERFERENCE_SCENARIO==1
        unsigned char txPowerTable [16] = {2,4,6,8,10,12,14,16,18,20,22,24,26,28,30,31};
        #elif INTERFERENCE_SCENARIO==2
        unsigned char txPowerTable [16];
        random_table_generator(txPowerTable);
        #else
        unsigned char txPowerTable = CONSTANT_TX_POWER;
        #endif
        
        
        //broadcast_open(&broadcast, 129, &broadcast_call);
	
	/* switch mac layer off, and turn radio on */

	
	NETSTACK_MAC.off(0);
	
        cc2420_on();
        
        #if INTERFERENCE_SCENARIO == 3
        
        cc2420_set_channel(SPECIFIED_CHANNEL);
        
        cc2420_set_txpower(CONSTANT_TX_POWER);
        
        printf ("Channel: %d, jamming on TX power: %d.\n",SPECIFIED_CHANNEL,CONSTANT_TX_POWER);

        #else
        
        cc2420_set_channel (node_id + NODE_ID_ALIGNMENT + 10);
        
        cc2420_set_txpower(txPowerTable[node_id + NODE_ID_ALIGNMENT - 1]);
        
        printf("Channel: %d, jamming on TX power: %d.\n",node_id + NODE_ID_ALIGNMENT + 10, txPowerTable[node_id + NODE_ID_ALIGNMENT - 1]);

        #endif
        
	while(1) {
            
            etimer_set (&jamTimer, 2);
            PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&jamTimer));
            packetbuf_copyfrom("H", 1);
            broadcast_send(&broadcast);
	}

	PROCESS_END();
}
/*---------------------------------------------------------------------------*/

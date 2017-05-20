#include <stdint.h>
#include <stdio.h>
#include <pru_cfg.h>
#include <pru_intc.h>
#include <rsc_types.h>
#include <pru_rpmsg.h>
#include "resource_table_1.h"

#include <pru_ctrl.h>

volatile register uint32_t __R30;
volatile register uint32_t __R31;
/* Host-1 Interrupt sets bit 31 in register R31 */
#define HOST_INT			((uint32_t) 1 << 31)

/* The PRU-ICSS system events used for RPMsg are defined in the Linux device tree
 * PRU0 uses system event 16 (To ARM) and 17 (From ARM)
 * PRU1 uses system event 18 (To ARM) and 19 (From ARM)
 */
#define TO_ARM_HOST			18
#define FROM_ARM_HOST			19

/*
 * Using the name 'rpmsg-client-sample' will probe the RPMsg sample driver
 * found at linux-x.y.z/samples/rpmsg/rpmsg_client_sample.c
 *
 * Using the name 'rpmsg-pru' will probe the rpmsg_pru driver found
 * at linux-x.y.z/drivers/rpmsg/rpmsg_pru.c
 */
#define CHAN_NAME			"rpmsg-pru"
//#define CHAN_NAME			"rpmsg-pru"

#define CHAN_DESC			"Channel 31"
#define CHAN_PORT			31

/*
 * Used to make sure the Linux drivers are ready for RPMsg communication
 * Found at linux-x.y.z/include/uapi/linux/virtio_config.h
 */
#define VIRTIO_CONFIG_S_DRIVER_OK	4


#define DATA_BIT               3//1
#define CLK_BIT                2//0
#define PRU_OCP_RATE_10MS         (200 * 1000 * 10)

uint8_t payload[RPMSG_BUF_SIZE];
uint8_t payload_falling[RPMSG_BUF_SIZE/2];

/*
 * main.c
 */
void main(void)
{
	struct pru_rpmsg_transport transport;
	uint16_t src, dst, len,counter,counter_n;//,count_sample,count_sample_n;
	volatile uint8_t *status,i;

volatile uint8_t dsc_data,dsc_clk, timeout,clk_high,amount;

	/* Allow OCP master port access by the PRU so the PRU can read external memories */
	CT_CFG.SYSCFG_bit.STANDBY_INIT = 0;

	/* Clear the status of the PRU-ICSS system event that the ARM will use to 'kick' us */
	CT_INTC.SICR_bit.STS_CLR_IDX = FROM_ARM_HOST;

	/* Make sure the Linux drivers are ready for RPMsg communication */
	status = &resourceTable.rpmsg_vdev.status;
	while (!(*status & VIRTIO_CONFIG_S_DRIVER_OK));

	/* Initialize the RPMsg transport structure */
	pru_rpmsg_init(&transport, &resourceTable.rpmsg_vring0, &resourceTable.rpmsg_vring1, TO_ARM_HOST, FROM_ARM_HOST);

	/* Create the RPMsg channel between the PRU and ARM user space using the transport structure. */
	while (pru_rpmsg_channel(RPMSG_NS_CREATE, &transport, CHAN_NAME, CHAN_DESC, CHAN_PORT) != PRU_RPMSG_SUCCESS);
	while (1) {
		/* Check bit 30 of register R31 to see if the ARM has kicked us */
		if (__R31 & HOST_INT) {
			/* Clear the event status */
			CT_INTC.SICR_bit.STS_CLR_IDX = FROM_ARM_HOST;
			/* Receive all available messages, multiple messages can be sent per kick */
			while (pru_rpmsg_receive(&transport, &src, &dst, payload, &len) == PRU_RPMSG_SUCCESS) {
//read
if(len==2)
{
	amount = payload[0] - 48; //convert char to int
}
else if(len >2)
{
        amount = payload[1] - 48; //convert char to int
        amount += (payload[0]-48)*10; //convert char to int
}

//write
counter = 0;
//payload[counter++] = (uint8_t)'B'; //'T'
timeout = 0;
clk_high = 0;
//count_sample = 0;
counter_n=0;
//count_sample_n = 0;
i = 0;
//1st phase
        /* Enable counter */
        PRU1_CTRL.CYCLE = 0;
        PRU1_CTRL.CTRL_bit.CTR_EN = 1;
        /* wait for DSC+CLOCK to stay low for 10ms */
        do {
                dsc_clk = ((__R31 & (1u << CLK_BIT)) > 0);
		if(dsc_clk == 1)
		{
			PRU1_CTRL.CTRL_bit.CTR_EN = 0;
			PRU1_CTRL.CYCLE=0;
			PRU1_CTRL.CTRL_bit.CTR_EN = 1;
		}
		timeout = PRU1_CTRL.CYCLE > PRU_OCP_RATE_10MS;
        } while (!timeout);
        PRU1_CTRL.CTRL_bit.CTR_EN = 0;

	//payload[counter++] = 84; //'T'
timeout = 0;
//next phase

        /* Restart the counter */
        PRU1_CTRL.CYCLE = 0;
        PRU1_CTRL.CTRL_bit.CTR_EN = 1;
        /* read dsc_data on rising edge of clock */
        do {
                dsc_clk = ((__R31 & (1u << CLK_BIT)) > 0);
		//Rising edge
		if (dsc_clk == 1 && !clk_high)
		{
			clk_high=1;
			dsc_data = (!((__R31 & (1u << DATA_BIT)) > 0));
			if(dsc_data == 0)
				payload[counter++] = 48; //0
			else
				payload[counter++] = 49; //1
			/*if(count_sample++ == 3)
			{
				count_sample = 0;
                                payload[counter++] = 32; //' '
			}*/
		}
		//Falling edge
		if (dsc_clk == 0 && clk_high)
                {
			clk_high=0;
			dsc_data = (!((__R31 & (1u << DATA_BIT)) > 0));
                        if(dsc_data == 0)
                                payload_falling[counter_n++] = 48; //0
                        else
                                payload_falling[counter_n++] = 49; //1
                        /*if(count_sample_n++ == 3)
                        {
                                count_sample_n = 0;
                                payload_falling[counter_n++] = 32; //' '
                        }*/

                }
                timeout = PRU1_CTRL.CYCLE > PRU_OCP_RATE_10MS;
        } while (counter<amount);//(!timeout);
        PRU1_CTRL.CTRL_bit.CTR_EN = 0;


/*
        payload[counter++] = (uint8_t)'e'; //84/'t'

	for(i=0;i<counter_n;i++)
		payload[counter++] = payload_falling[i];
        //payload[counter++] = 10; //LF
        //payload[counter++] = 13; //CR
*/
				/* Echo the message back to the same address from which we just received */
				pru_rpmsg_send(&transport, dst, src, payload, counter);
			}
		}
	}
}

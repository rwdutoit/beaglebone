#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <pru_cfg.h>
#include <pru_intc.h>
#include <rsc_types.h>
#include <pru_rpmsg.h>
#include "resource_table_0.h"

#include <pru_ctrl.h>

volatile register uint32_t __R30;//output
volatile register uint32_t __R31;//input
/* Host-1 Interrupt sets bit 31/30 (PRU0/PRU0) in register R31 */
#define HOST_INT			((uint32_t) 1 << 30)

/* The PRU-ICSS system events used for RPMsg are defined in the Linux device tree
 * PRU0 uses system event 16 (To ARM) and 17 (From ARM)
 * PRU0 uses system event 18 (To ARM) and 19 (From ARM)
 */
#define TO_ARM_HOST			16
#define FROM_ARM_HOST			17

/*
 * Using the name 'rpmsg-client-sample' will probe the RPMsg sample driver
 * found at linux-x.y.z/samples/rpmsg/rpmsg_client_sample.c
 *
 * Using the name 'rpmsg-pru' will probe the rpmsg_pru driver found
 * at linux-x.y.z/drivers/rpmsg/rpmsg_pru.c
 */
#define CHAN_NAME			"rpmsg-pru"
//#define CHAN_NAME			"find in hands on"

#define CHAN_DESC			"Channel 30"
#define CHAN_PORT			30

/*
 * Used to make sure the Linux drivers are ready for RPMsg communication
 * Found at linux-x.y.z/include/uapi/linux/virtio_config.h
 */
#define VIRTIO_CONFIG_S_DRIVER_OK	4

#define PRU_OCP_RATE_HZ 	(200 * 1000 * 1000 * 10)

#define OUT_BIT                14
#define TRIG_BIT               15
#define ECHO_BIT               15
#define PRU_OCP_RATE_10US      (200 * 10)


#define PIR_BIT                16
#define DATA_BIT               2
#define CLK_BIT                5
#define PRU_OCP_RATE_10MS      (200 * 1000 * 10)


char payload[RPMSG_BUF_SIZE];

int hc_sr04_measure_pulse(void)
{
	uint8_t echo, timeout;

	/* pulse the trigger for 10us */
	__R30 = __R30 | (1 << 15);
	__delay_cycles(PRU_OCP_RATE_10US);
	__R30 = __R30 & ~(1 << 15);

	/* Enable counter */
	PRU0_CTRL.CYCLE = 0;
	PRU0_CTRL.CTRL_bit.CTR_EN = 1;

	/* wait for ECHO to get high */
	do {
		echo =  (((__R31 & (1 << 15) ) >0 ));
		timeout = PRU0_CTRL.CYCLE > PRU_OCP_RATE_HZ;
	} while (!echo && !timeout);

	PRU0_CTRL.CTRL_bit.CTR_EN = 0;

	if (timeout)
		return -1;

	/* Restart the counter */
	PRU0_CTRL.CYCLE = 0;
	PRU0_CTRL.CTRL_bit.CTR_EN = 1;

	/* measure the "high" pulse length */
	do {
		echo =  (((__R31 & (1 << 15))>0));
		timeout = PRU0_CTRL.CYCLE > PRU_OCP_RATE_HZ;
	} while (echo && !timeout);

	PRU0_CTRL.CTRL_bit.CTR_EN = 0;
	if (timeout)
		return -1;

	uint64_t cycles = PRU0_CTRL.CYCLE;

	return cycles / (200);
}
static int measure_distance_mm(void)
{
	int t_us = hc_sr04_measure_pulse();
	int d_mm;

	/*
	 * Print the distance received from the sonar
	 * At 20 degrees in dry air the speed of sound is 3422 mm/sec
	 * so it takes 2.912 us to make 1 mm, i.e. 5.844 us for a
	 * roundtrip of 1 mm.
	 */
	d_mm = (t_us * 1000) / 5844;
	if (t_us < 0)
		d_mm = -1;

	return d_mm;
}
 /* reverse:  reverse string s in place */
 void reverse(char s[])
 {
     int i, j;
     char c;

     for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
         c = s[i];
         s[i] = s[j];
         s[j] = c;
     }
 }
 /* itoa:  convert n to characters in s */
 void itoa(int n, char s[])
 {
     int i, sign;

     if ((sign = n) < 0)  /* record sign */
         n = -n;          /* make n positive */
     i = 0;
     do {       /* generate digits in reverse order */
         s[i++] = n % 10 + '0';   /* get next digit */
     } while ((n /= 10) > 0);     /* delete it */
     if (sign < 0)
         s[i++] = '-';
     s[i] = '\0';
     reverse(s);
 }

void dsc_read(uint8_t amount,char output_char[])
{
volatile uint8_t dsc_data,dsc_clk, timeout,clk_high,counter,counter_n,i;
//write
counter = 0;
//output_char[counter++] = (uint8_t)'B'; //'T'
timeout = 0;
clk_high = 0;
//count_sample = 0;
counter_n=0;
//count_sample_n = 0;
i = 0;
//1st phase
        /* Enable counter */
        PRU0_CTRL.CYCLE = 0;
        PRU0_CTRL.CTRL_bit.CTR_EN = 1;
        /* wait for DSC+CLOCK to stay low for 10ms */

        do {
                dsc_clk = ((__R31 & (1u << CLK_BIT)) > 0);
                if(dsc_clk == 1)
                {
                        PRU0_CTRL.CTRL_bit.CTR_EN = 0;
                        PRU0_CTRL.CYCLE=0;
                        PRU0_CTRL.CTRL_bit.CTR_EN = 1;
                }
                timeout = PRU0_CTRL.CYCLE > PRU_OCP_RATE_10MS;
        } while (!timeout);
        PRU0_CTRL.CTRL_bit.CTR_EN = 0;

        //output_char[counter++] = 84; //'T'
timeout = 0;
//next phase

        /* Restart the counter */
        PRU0_CTRL.CYCLE = 0;
        PRU0_CTRL.CTRL_bit.CTR_EN = 1;
        /* read dsc_data on rising edge of clock */
        do {
                dsc_clk = ((__R31 & (1u << CLK_BIT)) > 0);
                //Rising edge
                if (dsc_clk == 1 && !clk_high)
                {
                        clk_high=1;
                        dsc_data = (!((__R31 & (1u << DATA_BIT)) > 0));
                        if(dsc_data == 0)
                                output_char[counter++] = 48; //0
                        else
                                output_char[counter++] = 49; //1
                        /*if(count_sample++ == 3)
                        {
                                count_sample = 0;
                                output_char[counter++] = 32; //' '
                        }*/
                }
                //Falling edge
                if (dsc_clk == 0 && clk_high)
                {
                        clk_high=0;
                        dsc_data = (!((__R31 & (1u << DATA_BIT)) > 0));
                        /*
			if(dsc_data == 0)
                                output_char_falling[counter_n++] = 48; //0
                        else
                                output_char_falling[counter_n++] = 49; //1
                        */
			/*if(count_sample_n++ == 3)
                        {
                                count_sample_n = 0;
                                output_char_falling[counter_n++] = 32; //' '
                        }*/

                }
                timeout = PRU0_CTRL.CYCLE > PRU_OCP_RATE_10MS;
        } while (counter<amount);//(!timeout);
        PRU0_CTRL.CTRL_bit.CTR_EN = 0;


/*
        output_char[counter++] = (uint8_t)'e'; //84/'t'

        for(i=0;i<counter_n;i++)
                output_char[counter++] = output_char_falling[i];
        //output_char[counter++] = 10; //LF
        //output_char[counter++] = 13; //CR
*/

}
/*
 * main.c
 */
void main(void)
{
	struct pru_rpmsg_transport transport;
	uint16_t src, dst, len;
	uint8_t temp;
	volatile uint8_t *status,i,amount;

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

				dsc_read(amount,payload);
				/* Echo the message back to the same address
				 * from which we just received */
				int d_mm = measure_distance_mm();
				/* there is no room in IRAM for iprintf */
				char number[5];
				itoa(d_mm, number);//, 10);
				//additional PIR
				/*
				temp = (!((__R31 & (1u << PIR_BIT)) > 0));
	                        if(temp == 0)
				{
        	                        payload[0] = 48; //0
                	        	__R30 = __R30 | (1 << OUT_BIT);
				}
				else
				{
                        	        payload[0] = 49; //1
				        __R30 = __R30 & ~(1 << OUT_BIT);

				}
				*/
				for(i=0;i<5;i++)
					payload[amount+i] = number[i];
				pru_rpmsg_send(&transport, dst, src,
					payload, strlen(payload) + 1);
			}
		}
	}
}

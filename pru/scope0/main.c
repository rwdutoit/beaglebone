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

//ADC
#define PIR_BIT                16
#define DATA_BIT               3
#define CLK_BIT                2
#define PRU_OCP_RATE_10MS      (200 * 1000 * 10)

char payload[RPMSG_BUF_SIZE];
char payloadChannels[2][RPMSG_BUF_SIZE/2];

#define PRU0_ARM_INTERRUPT 19

#define ADC_BASE 0x44e0d000

#define CONTROL 0x0040
#define SPEED   0x004c
#define STEP1   0x0064
#define DELAY1  0x0068
#define STATUS  0x0044
#define STEPCONFIG  0x0054
#define FIFO0COUNT  0x00e4

#define ADC_FIFO0DATA   (ADC_BASE + 0x0100)


 /* reverse:  reverse string s in place */
 void reverse(char s[])
 {
     int i, j;
     char c;

     for (i = 0, j = strlen(s)-1; i<j; i++, j--) 
     {
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
     do
     {
       /* generate digits in reverse order */
         s[i++] = n % 10 + '0';   /* get next digit */
     } while ((n /= 10) > 0);     /* delete it */
     if (sign < 0)
         s[i++] = '-';
     s[i] = '\0';
     reverse(s);
 }

void dsc_read_test(uint8_t amount)
{
	int i = 0;
	for (i =0;i < amount; i++)
{
	payload[i] = i + 48;
}
	//strncpy(payloadLong, "hello", amount);
        return;
}

void dsc_read(int amount, int counters[2])
{
	volatile int dsc_data, dsc_clk, timeout, clk_high, data_high, counter = 0, counter_n =0;

	counter = 0;
	counter_n =0;
	timeout = 0;
	clk_high = 0;
	data_high = 0;

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
        clk_high = 0;
	data_high = 0;
        /* read dsc_data on rising edge of clock */
        do {
                dsc_clk = ((__R31 & (1u << CLK_BIT)) > 0);
                //Rising edge
                if (dsc_clk == 1 && !clk_high)
                {
                        clk_high=1;
			payloadChannels[0][counter++] = PRU0_CTRL.CYCLE & 0xFF;
			payloadChannels[0][counter++] = (PRU0_CTRL.CYCLE >> 8) & 0xFF;
			payloadChannels[0][counter++] = (PRU0_CTRL.CYCLE >> 16) & 0xFF;
			payloadChannels[0][counter++] = (PRU0_CTRL.CYCLE >> 24) & 0xFF;
                }
                //Falling edge
                if (dsc_clk == 0 && clk_high)
                {
                        clk_high=0;
			payloadChannels[0][counter++] = PRU0_CTRL.CYCLE & 0xFF;
			payloadChannels[0][counter++] = (PRU0_CTRL.CYCLE >> 8) & 0xFF;
			payloadChannels[0][counter++] = (PRU0_CTRL.CYCLE >> 16) & 0xFF;
			payloadChannels[0][counter++] = (PRU0_CTRL.CYCLE >> 24) & 0xFF;
                }


                dsc_data = ((__R31 & (1u << DATA_BIT)) > 0);
                //Rising edge
                if (dsc_data == 1 && !data_high)
                {
                        data_high=1;
			payloadChannels[1][counter_n++] = PRU0_CTRL.CYCLE & 0xFF;
			payloadChannels[1][counter_n++] = (PRU0_CTRL.CYCLE >> 8) & 0xFF;
			payloadChannels[1][counter_n++] = (PRU0_CTRL.CYCLE >> 16) & 0xFF;
			payloadChannels[1][counter_n++] = (PRU0_CTRL.CYCLE >> 24) & 0xFF;
                }
                //Falling edge
                if (dsc_data == 0 && data_high)
                {
                        data_high=0;
			payloadChannels[1][counter_n++] = PRU0_CTRL.CYCLE & 0xFF;
			payloadChannels[1][counter_n++] = (PRU0_CTRL.CYCLE >> 8) & 0xFF;
			payloadChannels[1][counter_n++] = (PRU0_CTRL.CYCLE >> 16) & 0xFF;
			payloadChannels[1][counter_n++] = (PRU0_CTRL.CYCLE >> 24) & 0xFF;
                }
                //timeout = PRU0_CTRL.CYCLE > PRU_OCP_RATE_10MS;
        } while (  counter < amount && counter_n < amount );  //|| counter_n > amount ) );//(!timeout);
        PRU0_CTRL.CTRL_bit.CTR_EN = 0;
	counters[0] = counter;
	counters[1] = counter_n;
}
/*
 * main.c
 */
void main(void)
{
	struct pru_rpmsg_transport transport;
	uint16_t src, dst, len;
	volatile uint8_t *status,i,j;
        int channels = 2;
	int amount = 0;
	int offset = 4;

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
	while (1)
	{
		/* Check bit 30 of register R31 to see if the ARM has kicked us */
		if (__R31 & HOST_INT)
		{
			/* Clear the event status */
			CT_INTC.SICR_bit.STS_CLR_IDX = FROM_ARM_HOST;
			/* Receive all available messages, multiple messages can be sent per kick */
			while (pru_rpmsg_receive(&transport, &src, &dst, payload, &len) == PRU_RPMSG_SUCCESS)
			{

				if (len == 1)
				{
					amount = payload[0] -48;
				}
				else if (len == 2)
				{
					amount = payload[1] - 48;
					amount += (payload[0]-48) * 10;
				}
				else if (len > 2)
				{
					amount = payload[2] - 48;
					amount += (payload[1]-48) * 10;
					amount += (payload[0]-48) * 100;
				}

				int counters[2];
				dsc_read(amount, counters);

				offset = 4;
				amount = counters[0] + counters[1] + offset;
				int count_total = offset;
				for(i=0 ; i< channels; i++)
				{
					for(j=0; j < counters[i]; j++)
					{
						payload[count_total++] = payloadChannels[i][j];
					}
				}
				//dsc_read_test(amount);
				//amount= payload[0];

				payload[0] = counters[0] & 0xFF;
				payload[1] = (counters[0] >> 8) & 0xFF;
				payload[2] = (counters[1]) & 0xFF;
				payload[3] = (counters[1] >> 8) & 0xFF;

				pru_rpmsg_send(&transport, dst, src,
					payload, amount);
			}
		}
	}
}

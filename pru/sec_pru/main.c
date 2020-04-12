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

/*
#define temp0 r1
#define temp1 r2
#define temp2 r3
#define temp3 r4
#define temp4 r5
#define adc_base r6
#define fifo0data r7
#define out_buf r8
#define locals r9
#define cap_delay r14
*/

unsigned long read_adc()//long *cap_delay, long *ticks)
{
int i=0;
register unsigned long temp0 =0;
register unsigned long temp1 =0;
register unsigned long temp2 =0;
register unsigned long temp3 =0;

unsigned long cap_delay=0;
unsigned long ticks=0;
register unsigned long adc_val;
unsigned long channel=0;
//unsigned long * locals = (volatile unsigned long*) 0;

register unsigned long *adc_base = (unsigned long*) ADC_BASE;
register unsigned long *fifo0data = (unsigned long*) ADC_FIFO0DATA;

/*
	locals = 0;
	temp0 = *locals
	temp1 = 0xbeef1965;
	if(temp0 != temp1)
		return -1;
*/
//disable ADC
	temp0 = *(adc_base + CONTROL);
	temp1 = 0x1;
	temp1 = ~temp1;
	temp0 = temp0 & temp1;
	*(adc_base + CONTROL) = temp0; //disable
//ADC speed
	temp0 = 0;
	*(adc_base + SPEED) = temp0; //full speed

//cap_delay
//	cap_delay = *(locals + 0xc4);

//setup step and delay
temp0=STEP1;
temp1=0;
temp2=0;
for(i=0;i<8;i++) //channels
{
	temp3 = temp1 << 19;
	*(adc_base + temp0 ) = temp3; //step-config-#
	temp0 += 4;//goto next register
	*(adc_base + temp0 ) = temp2; //step-delay-#
	temp1 += 1;//increment value for SEL_INP
	temp0 += 4;//goto next register
}

//Enable ADC
	temp0 = *(adc_base + CONTROL); //
	temp0 |= 0x7;
	*(adc_base + CONTROL) = temp0; //


	if(cap_delay != 0)//capture_delay
	{
		temp0 = cap_delay;
		while(temp0 != 0) temp0 -= 1;
	}

	//no-delay
	temp0 = 0x1fe;//  0x2; //1 step // 0x1fe; //8-steps
	*(adc_base + STEPCONFIG) = temp0; //enable 8-steps. First bit0 =0
/*
	temp0 = *(locals + 0x14); //read runtime flags
	if( (temp0 & 0x1) != 0)
		return -1;
	else
	{
		//no-scope
		temp0 = *(locals+0x04)
	}
*/

	ticks++;//increment ticks

//wait for fifo
	temp0 = 0;
	while(temp0 < 8) //wait for 8-words in fifo register
		temp0 = *(adc_base + FIFO0COUNT);

for(i=0;i< 8;i++)//channels
{
//read all fifo
	adc_val = *(fifo0data);
	channel = adc_val >> 16;
	channel &= 0xf;
	temp1 = 0xfff;
	adc_val &= temp1;

//not ENC0 or ENC1
	//load data

//next channel
	temp0 = temp0 -1;
}

	return adc_val;
}

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

void dsc_read(uint8_t amount,unsigned long *output_long)
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
*output_long =0;
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
                                *output_long &= ~(1<<counter++) ;//output_char[counter++] = 48; //0
                        else
                                *output_long |= (1<<counter++); //output_char[counter++] = 49; //1
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

	//output_char[--counter] = (char)'\0';
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
	int counter =0;
	unsigned long *output_long;

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

/*
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
*/
amount = payload[0];
counter=0;
				dsc_read(amount,output_long); //payload);
				//output_long = 0xaaaa;
				payload[counter++] = (uint8_t)(*output_long);
				payload[counter++] = (uint8_t)(*output_long >> 8);
				payload[counter++] = (uint8_t)(*output_long >> 16);
				payload[counter++] = (uint8_t)(*output_long >> 24);
				/* Echo the message back to the same address
				 * from which we just received */
//				int d_mm = measure_distance_mm();
//				payload[counter++] = (uint8_t)d_mm;
//				payload[counter++] = (uint8_t)(d_mm >>8);
				/* there is no room in IRAM for iprintf */
				//char number[6];
				//itoa(d_mm, number);//, 10);
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
				//for(i=0;i<6;i++)
				//	payload[amount+i] = number[i];
				//amount += 6;

				unsigned long test = 1234;
				//test = read_adc();
				payload[counter++] = (uint8_t)test;
				payload[counter++] = (uint8_t)(test >> 8);

				//itoa(test,number);
				//for(i=0;i<6;i++)
				//	payload[amount+i] = number[i];
				//payload[amount++] = '\0';

				pru_rpmsg_send(&transport, dst, src,
					payload, amount); //strlen(payload) + 1);
			}
		}
	}
}

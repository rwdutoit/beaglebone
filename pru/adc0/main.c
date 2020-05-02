#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <pru_cfg.h>
#include <pru_intc.h>
#include <rsc_types.h>
#include <pru_rpmsg.h>
#include "resource_table_0.h"

#include <pru_ctrl.h>
#include <sys_tscAdcSs.h>

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

#define PRU_OCP_RATE_HZ 	(200 * 1000 * 1000)

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
//short adc1[RPMSG_BUF_SIZE/2];

#define PRU0_ARM_INTERRUPT 19


#define ADC_BASE 0x44e0d000 //ADC_TSC

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

read_adc()//int *cap_delay, int *ticks)
{
	uint32_t status = 0;
	uint32_t increment = 0;
	uint32_t fifoCount = 0;
	uint32_t channel = 0;
	uint32_t enable_chan1 = 1<<1;
	uint32_t adc_val = 0;

//disable ADC
	ADC_TSC.CTRL_bit.ENABLE =  0;
//ADC speed
	ADC_TSC.ADC_CLKDIV_bit.ADC_CLKDIV = 0;
//setup step and delay
//ADC-1
	ADC_TSC.STEPCONFIG1 = 0;
	ADC_TSC.STEPCONFIG1_bit.SEL_INP_SWC_3_0 = 0; //0 = Channel 1
	ADC_TSC.STEPDELAY1 = 0;
/*	//Suppose to set all 8 ADC
	ADC_TSC.STEPCONFIG2 = 0;
	ADC_TSC.STEPDELAY2_bit.SEL_INP_SWC_3_0 = 1;
	ADC_TSC.STEPCONFIG3 = 0;
	ADC_TSC.STEPDELAY3_bit.SEL_INP_SWC_3_0 = 2;
	ADC_TSC.STEPCONFIG4 = 0;
	ADC_TSC.STEPDELAY4_bit.SEL_INP_SWC_3_0 = 3;
*/

//Enable ADC
	ADC_TSC.CTRL_bit.ENABLE =  1;
	ADC_TSC.CTRL_bit.STEP_ID_TAG = 1;
	ADC_TSC.CTRL_bit.STEPCONFIG_WRITEPROTECT_N_ACTIVE_LOW = 1;

	do
	{
		adc_val = 0;
	//no-delay
	//Enable STEP1
		ADC_TSC.STEPENABLE =  enable_chan1;
		status = ADC_TSC.ADCSTAT_bit.STEP_ID;
	//wait for fifo
		do{
			fifoCount = ADC_TSC.FIFO0COUNT_bit.WORDS_IN_FIFO0;
		}while (fifoCount < 1);
	//channel number
		channel = ADC_TSC.FIFO0DATA_bit.ADCCHNLID;
	//read all fifo
		adc_val = ADC_TSC.FIFO0DATA_bit.ADCDATA;
		payload[increment++] = adc_val;
		payload[increment++] = adc_val >> 8;
	}while (increment < 480);

}

/*
 * main.c
 */
void main(void)
{
	struct pru_rpmsg_transport transport;
	uint16_t src, dst, len;
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

				read_adc();
				pru_rpmsg_send(&transport, dst, src,
					payload, 480); //strlen(payload) + 1);
			}
		}
	}
}

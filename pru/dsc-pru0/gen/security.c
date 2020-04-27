
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/poll.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/mman.h>

#define MAX_BUFFER_SIZE		512
char readBuf0[MAX_BUFFER_SIZE];
//char readBuf[MAX_BUFFER_SIZE];

#define NUM_MESSAGES		32
#define DEVICE_PRU0		"/dev/rpmsg_pru30"
//#define DEVICE_PRU1		"/dev/rpmsg_pru31"
#define BIT_COUNT		62 + 36

#define ADC_RAW 		"/sys/bus/iio/devices/iio:device0/in_voltage0_raw"

#define PROG_MAX	25

static volatile int keepRunning = 1;

#define GPIO_START	0x4804c000
#define GPIO_END	0x4804e000
#define GPIO_SIZE	(GPIO_END - GPIO_START)
#define GPIO_SET	0x194
#define GPIO_CLEAR	0x190

#define ADC_BASE 	0x44e0d000
#define ADC_END         0x44e0f000
#define ADC_SIZE 	(ADC_END - ADC_BASE)

#define CONTROL 	0x0040
#define SPEED   	0x004c
#define STEPCONFIG1   	0x0064
#define DELAY1  	0x0068
#define STATUS  	0x0044
#define STEPENABLE  	0x0054
#define FIFO0COUNT  	0x00e4

#define ADC_FIFO0DATA   0x0100

void intHandler(int sig) {
    keepRunning = 0;
}

int main(void)
{
	struct pollfd pollfds[3];
	int i,col,distance,prev_d,count,bits_int; //,adc_val;
	unsigned long adc_val[8];
	int result = 0;
	int motion[8];
	volatile int door_count[8],door_acc[8],isOpen[8],once[8],temp[8];

for(i=0;i<sizeof(isOpen);i++)
	isOpen[i] = 0;

char progress[PROG_MAX];
int prog_count;

	double alpha,avg_d,current;

FILE *gpioInput[2];
gpioInput[0] = NULL;
gpioInput[1] = NULL;
FILE *gpioOutput = NULL;
FILE *adcInput = NULL;

char buffer[10];
int gpio_val,gpio_col[2],gpio_once_r,gpio_once_w;

gpio_once_r =0;
gpio_once_w =0;

door_count[0] = 0;
door_acc[0] = 0;
isOpen[0] = 0;
once[0] = 0;
temp[0]=0;

alpha = 0.1;
avg_d=0;

i=0;
count=0;
col=42;
bits_int=0;
signal(SIGINT, intHandler);
	/* Open the rpmsg_pru character device file */
	pollfds[0].fd = open(DEVICE_PRU0, O_RDWR);
	if (pollfds[0].fd < 0) 
	{
		printf("Failed to open %s\n", DEVICE_PRU0);
		return -1;
	}
	/* The RPMsg channel exists and the character device is opened */
	printf("Opened %s, sending %d messages\n\n", DEVICE_PRU0, NUM_MESSAGES);

	//for (i = 0; i < NUM_MESSAGES; i++) 
	while (keepRunning) 
	{
		count++;
		usleep(500000);
		/* Send 'hello world!' to the PRU through the RPMsg channel */
		//result = write(pollfds[1].fd, "12", 3);
		char send_val = 20;
		result = write(pollfds[0].fd, &send_val, 3);
		int gpio_val = 0;
		int gpio_val2 = 0;


int adc_col=42;
		result = read(pollfds[0].fd, readBuf0, MAX_BUFFER_SIZE);
		if (result > 0)
		{
			bits_int = (int)readBuf0[0] | ((int)readBuf0[1] << 8) | ((int)readBuf0[2] << 16) | ((int)readBuf0[3] << 24);

			for(i=0;i<8;i++)
			{
				if( (bits_int >> i) & 0x01 )// (readBuf0[i]-48) > 0)
				{
					isOpen[i] = 0;
					motion[i] = 42;//green
				}
				else
				{
					isOpen[i] = 1;
					motion[i] = 41;//red
				}
			}
                        
			printf("%c[H%c[J Reading:%d \n\r [ %x %x %x %x ] Movement:\n\r"
				"%c[%dm Upstairs:		%d %c[40m\n\r"
				"%c[%dm Panic:			%d %c[40m\n\r"
				"%c[%dm Lounge+ Kitchen door:	%d %c[40m\n\r"
				"%c[%dm Garage door: 		%d %c[40m\n\r"
				"%c[%dm Sliding door: 		%d %c[40m\n\r"
				"%c[%dm Lounge PIR: 		%d %c[40m\n\r"
				"%c[%dm Passage PIR: 		%d %c[40m\n\r"
				"%c[%dm Bedroom PIR: 		%d %c[40m\n\r",
//				"%c[%dm next 8:	 		%d %c[40m\n\r",
//				"%c[%dm next 9:	 		%d %c[40m\n\r",
//				"%c[%dm next 10	 		%d %c[40m\n\r",
//				"%c[%dm next 11: 		%d %c[40m\n\r",
//				"%c[%dm next 12: 		%d %c[40m\n\r",
//				"%c[%dm next 13: 		%d %c[40m\n\r",
//				"%c[%dm next 14: 		%d %c[40m\n\r",
//				"%c[%dm next 15: 		%d %c[40m\n\r",
//				"%c[%dm next 16: 		%d %c[40m\n\r",
//				"%c[%dm next 17: 		%d %c[40m\n\r",
//				"%c[%dm next 18: 		%d %c[40m\n\r",
//				"%c[%dm next 19: 		%d %c[40m\n\r",
//				"%c[%dm next 20: 		%d %c[40m\n\r",
//				"%c[%dm next 21: 		%d %c[40m\n\r",
//				"%c[%dm next 22: 		%d %c[40m\n\r",
	                        27,27,count,
				readBuf0[0],readBuf0[1],readBuf0[2],readBuf0[3],
				27,motion[0],isOpen[0],27, //readBuf0[1]-48,27,
				27,motion[1],isOpen[1],27, //readBuf0[1]-48,27,
				27,motion[2],isOpen[2],27, //readBuf0[2]-48,27,
				27,motion[3],isOpen[3],27, //readBuf0[3]-48,27,
				27,motion[4],isOpen[4],27, //,readBuf0[4]-48,27,
				27,motion[5],isOpen[5],27, //,readBuf0[5]-48,27,
				27,motion[6],isOpen[6],27, //,readBuf0[6]-48,27,
				27,motion[7],isOpen[7],27,
				27,motion[8],isOpen[8],27);
//				27,motion[9],isOpen[9],27,
//				27,motion[10],isOpen[10],27,
//				27,motion[11],isOpen[11],27,
//				27,motion[12],isOpen[12],27,
//				27,motion[13],isOpen[13],27,
//				27,motion[14],isOpen[14],27,
//				27,motion[15],isOpen[15],27,
//				27,motion[16],isOpen[16],27,
//				27,motion[17],isOpen[17],27,
//				27,motion[18],isOpen[18],27,
//				27,motion[19],isOpen[19],27,
//				27,motion[20],isOpen[20],27,
//				27,motion[21],isOpen[21],27,
//				27,motion[22],isOpen[22],27);
//			fflush(stdout);
		}
	}

	/* Received all the messages the example is complete */
	printf("%c[40m\r\nClosing %s\n",27,DEVICE_PRU0);

	/* Close the rpmsg_pru character device file */
	close(pollfds[0].fd);
        close(pollfds[1].fd);
	fclose(gpioInput[0]);
	fclose(gpioInput[1]);
	fclose(gpioOutput);
	return 0;
}


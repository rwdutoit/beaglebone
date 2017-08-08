#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/poll.h>
#include <signal.h>
#include <stdlib.h>

#define MAX_BUFFER_SIZE		512
char readBuf0[MAX_BUFFER_SIZE];
//char readBuf[MAX_BUFFER_SIZE];

#define NUM_MESSAGES		30
#define DEVICE_PRU0		"/dev/rpmsg_pru30"
//#define DEVICE_PRU1		"/dev/rpmsg_pru31"
#define BIT_COUNT		62 + 36

#define ADC_RAW 		"/sys/bus/iio/devices/iio:device0/in_voltage0_raw"

#define PROG_MAX	25

static volatile int keepRunning = 1;

void intHandler(int dummy) {
    keepRunning = 0;
}

int main(void)
{
	struct pollfd pollfds[3];
	int i,col,distance,prev_d,count;
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
signal(SIGINT, intHandler);
	/* Open the rpmsg_pru character device file */
	pollfds[0].fd = open(DEVICE_PRU0, O_RDWR);
	if (pollfds[0].fd < 0) {
		printf("Failed to open %s\n", DEVICE_PRU0);
		return -1;
	}
	/*
	pollfds[1].fd = open(DEVICE_PRU1, O_RDWR);
	if (pollfds[1].fd < 0) {
		printf("Failed to open %s\n", DEVICE_PRU1);
		return -1;
	}
	*/

	if( (gpioInput[0] = fopen("/sys/class/gpio/gpio20/value","rb")) == NULL)
	{
		printf("Failed to open GPIO input file \n");
                return -1;
	}
	if( (gpioInput[1] = fopen("/sys/class/gpio/gpio26/value","rb")) == NULL)
	{
		printf("Failed to open GPIO input file \n");
                return -1;
	}
	if( (gpioOutput = fopen("/sys/class/gpio/gpio44/value","w")) == NULL)
	{
		printf("Failed to open GPIO output file \n");
		return -1;
	}
/*	
        if( (adcInput = fopen(ADC_RAW,"rb")) == NULL)
        {
                printf("Failed to open ADC_RAW input file \n");
                return -1;
        }
*/	

	/* The RPMsg channel exists and the character device is opened */
	printf("Opened %s, sending %d messages\n\n", DEVICE_PRU0, NUM_MESSAGES);

	//for (i = 0; i < NUM_MESSAGES; i++) 
	while (keepRunning) 
	{
		count++;
		usleep(50000);
		/* Send 'hello world!' to the PRU through the RPMsg channel */
		//result = write(pollfds[1].fd, "12", 3);
		result = write(pollfds[0].fd, "20", 3);
		//if (result > 0)
		//	printf("Message %d: Sent to PRU\n", i);

		fseek(gpioOutput,0,SEEK_SET);

		fseek(gpioInput[0],0,SEEK_SET);
		fread(buffer,sizeof(char),sizeof(buffer)-1,gpioInput[0]);
		int gpio_val = atoi(buffer);

		fseek(gpioInput[1],0,SEEK_SET);
		fread(buffer,sizeof(char),sizeof(buffer)-1,gpioInput[1]);
		int gpio_val2 = atoi(buffer);

int adc_col=42;
/*
int min,max,diff,adc_col;
min=99999;max=0;diff=0;adc_col=0;
for(i=0;i<25;i++)
{
		fseek(adcInput,0,SEEK_SET);
		fread(buffer,sizeof(char),sizeof(buffer)-1,adcInput);
		int adc_val = atoi(buffer);
		if(adc_val<min)min=adc_val;
		if(adc_val>max)max=adc_val;
}
diff=max-min;
if(avg_d<100)
	adc_col=42;//green
else
	adc_col=41;//red
*/

		/* Poll until we receive a message from the PRU and then print it */
		//result = read(pollfds[1].fd, readBuf, MAX_BUFFER_SIZE);
		result = read(pollfds[0].fd, readBuf0, MAX_BUFFER_SIZE);
		if (result > 0)
			/*printf("\r%02d: %s",i,readBuf);*/

			distance = atoi(&readBuf0[20]);
/*
			current = (double)diff;
			avg_d = alpha*current + (1-alpha)*avg_d; //exponential moving avg
*/			

			//if(count==1)
			//	prev_d = distance;
			//if((distance-prev_d) > 100)
			//	distance = prev_d;
			if(distance > 3300)
				distance = 3300;
			if(distance<300)
				col = 41;//red
			else if(distance<800)
				col = 42;//green
			else if (distance<2000)
				col = 44;//blue
			for(i=0;i<8;i++)
			{
				if( (readBuf0[i]-48) > 0)
					motion[i] = 42;//green
				else
					motion[i] = 41;//red
			}
			if(gpio_val == 0)
			{
				gpio_col[0] = 42;//green
				if(gpio_once_r ==0)
				{
					int gpioOff =0;
					//fwrite(&gpioOff,sizeof(char),1,gpioOutput);
					fprintf(gpioOutput,"%d",0);
					gpio_once_w = 0;
				}
				gpio_once_r = 1;
			}
			else
			{
				gpio_col[0] = 41;//red
				if(gpio_once_w ==0)
				{
					int gpioOn =1;
					//fwrite(&gpioOn,sizeof(char),1,gpioOutput);
					fprintf(gpioOutput,"%d",1);
					gpio_once_r = 0;
				}
				gpio_once_w =1;
			}
			if(gpio_val2 == 1)
				gpio_col[1] = 42;//green
			else
				gpio_col[1] = 41;//red
			printf("%c[H%c[J Reading:%d \n\r Movement:\n\r"
				"%c[%dm Lounge door:		%d %c[40m\n\r"
				"%c[%dm Upstairs:		%d %c[40m\n\r"
				"%c[%dm Panic:			%d %c[40m\n\r"
				"%c[%dm Lounge+ Kitchen door:	%d %c[40m\n\r"
				"%c[%dm Garage door: 		%d %c[40m\n\r"
				"%c[%dm Sliding door: 		%d %c[40m\n\r"
				"%c[%dm Lounge PIR: 		%d %c[40m\n\r"
				"%c[%dm Passage PIR: 		%d %c[40m\n\r"
				"%c[%dm Bedroom PIR: 		%d %c[40m\n\r",
                                27,27,count,
				27,gpio_col[1],gpio_val2,27, //readBuf0[0]-48,27,
				27,motion[0],isOpen[0],27, //readBuf0[1]-48,27,
				27,motion[1],isOpen[1],27, //readBuf0[1]-48,27,
				27,motion[2],isOpen[2],27, //readBuf0[2]-48,27,
				27,motion[3],isOpen[3],27, //readBuf0[3]-48,27,
				27,motion[4],isOpen[4],27, //,readBuf0[4]-48,27,
				27,motion[5],isOpen[5],27, //,readBuf0[5]-48,27,
				27,motion[6],isOpen[6],27, //,readBuf0[6]-48,27,
				27,motion[7],isOpen[7],27); //,readBuf0[7]-48,27);
			printf("%c[%dm Garage PIR: 		%d %c[40m\n\r",27,gpio_col[0],gpio_val,27);
//			printf("%c[%dm CT ADC_raw: 		%d %c[40m\n\r",27,adc_col,(int)avg_d,27);
			printf("%c[%dm Distance: 		%d mm %c[40m\n\r",27,col,distance,27);

		prog_count = distance * PROG_MAX / 3300;
		for(i=0;i<prog_count;i++)
			progress[i]= '#';
		for(i=prog_count;i<PROG_MAX;i++)
			progress[i]=' ';
		printf("%s",progress);
		prev_d = distance;
		fflush(stdout);
for(i=0;i<8;i++)
{
		if(door_count[i]++ < 20)
                {
                        door_acc[i] += (readBuf0[i]- 48);
                }
                else
                {
                        if (door_acc[i] ==20)
                        {
				temp[i] = door_acc[i];
                                isOpen[i] = 0;
				once[i]=0;
                        }
                        else
                        {
				temp[i] = door_acc[i];
				isOpen[i] = 1;
                                if(once[i]==0)
                                {
                                        once[i] =1;
					if(i==3)
					{
                                        popen("xset -display :0.0 dpms force on","r");//interrupt screensaver
                                        printf("\r\nScreen on\r\n");
					}
                                }
                        }
                        door_count[i]=0;
			door_acc[i]=0;
                }
}
                /*
                if(readBuf0[3] == 48)
                {
                        popen("xset -display :0.0 dpms force on","r");//interrupt screensaver
                        printf("Screen on");
                }
                */
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


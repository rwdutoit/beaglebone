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


static volatile int keepRunning = 1;

void intHandler(int dummy) {
    keepRunning = 0;
}

int main(void)
{
	struct pollfd pollfds[2];
	int i,col,distance,prev_d,count;
	int result = 0;
	int motion[8];
//	double alpha,prev,current;
//alpha = 0.5;
//prev=0;
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
	
	/* The RPMsg channel exists and the character device is opened */
	printf("Opened %s, sending %d messages\n\n", DEVICE_PRU0, NUM_MESSAGES);

	//for (i = 0; i < NUM_MESSAGES; i++) 
	while (keepRunning) 
	{
		count++;
		usleep(500000);
		/* Send 'hello world!' to the PRU through the RPMsg channel */
		//result = write(pollfds[1].fd, "12", 3);
		result = write(pollfds[0].fd, "20", 3);
		//if (result > 0)
		//	printf("Message %d: Sent to PRU\n", i);

		/* Poll until we receive a message from the PRU and then print it */
		//result = read(pollfds[1].fd, readBuf, MAX_BUFFER_SIZE);
		result = read(pollfds[0].fd, readBuf0, MAX_BUFFER_SIZE);
		if (result > 0)
			/*printf("\r%02d: %s",i,readBuf);*/
			distance = atoi(&readBuf0[20]);
			//current = (double)distance;
			//current = alpha*current + (1-alpha)*prev; //exponential moving avg
			//prev = distance;
			if(distance > 3400)
				distance = prev_d;
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
			printf("%c[H%c[J Reading:%d \n\r Movement:\n\r"
				"%c[%dm Passage: 	%d %c[40m\n\r"
				"%c[%dm Sliding door: 	%d %c[40m\n\r"
				"%c[%dm Garage+ Kitchen:%d %c[40m\n\r"
				"%c[%dm Garage door: 	%d %c[40m\n\r"
				"%c[%dm Bedroom: 	%d %c[40m\n\r"
				"%c[%dm Upstairs: 	%d %c[40m\n\r"
				"%c[%dm ?:	 	%d %c[40m\n\r"
				"%c[%dm Panic: 		%d %c[40m\n\r"
				"%c[%dm Distance: 	%d mm %c[40m", // string:%s",//  %c[2K %s %c[1A",
				27,27,count,
				27,motion[0],readBuf0[0]-48,27,
				27,motion[1],readBuf0[1]-48,27,
				27,motion[2],readBuf0[2]-48,27,
				27,motion[3],readBuf0[3]-48,27,
				27,motion[4],readBuf0[4]-48,27,
				27,motion[5],readBuf0[5]-48,27,
				27,motion[6],readBuf0[6]-48,27,
				27,motion[7],readBuf0[7]-48,27,
				27,col,distance,27);//,readBuf0);
				//27,27);
                        //printf("\r %c[2K %02d: %s",27,i,readBuf);
		prev_d = distance;
		fflush(stdout);
		if(readBuf0[3] == 48)
		{
			popen("xset -display :0.0 dpms force on","r");//interrupt screensaver
			printf("Screen on");
		}
	}

	/* Received all the messages the example is complete */
	printf("%c[40m\r\nClosing %s\n",27,DEVICE_PRU0);

	/* Close the rpmsg_pru character device file */
	close(pollfds[0].fd);
        close(pollfds[1].fd);
	return 0;
}


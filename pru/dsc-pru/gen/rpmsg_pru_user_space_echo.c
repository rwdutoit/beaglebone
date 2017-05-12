#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/poll.h>

#define MAX_BUFFER_SIZE		512
char readBuf[MAX_BUFFER_SIZE];

#define NUM_MESSAGES		30
#define DEVICE_NAME		"/dev/rpmsg_pru31"
#define BIT_COUNT		62 + 36

int main(void)
{
	struct pollfd pollfds[1];
	int i;
	int result = 0;

	/* Open the rpmsg_pru character device file */
	pollfds[0].fd = open(DEVICE_NAME, O_RDWR);

	/*
	 * If the RPMsg channel doesn't exist yet the character device
	 * won't either.
	 * Make sure the PRU firmware is loaded and that the rpmsg_pru
	 * module is inserted.
	 */
	if (pollfds[0].fd < 0) {
		printf("Failed to open %s\n", DEVICE_NAME);
		return -1;
	}

	/* The RPMsg channel exists and the character device is opened */
	printf("Opened %s, sending %d messages\n\n", DEVICE_NAME, NUM_MESSAGES);

	for (i = 0; i < NUM_MESSAGES; i++) 
	{
		usleep(1000000);
		/* Send 'hello world!' to the PRU through the RPMsg channel */
		result = write(pollfds[0].fd, "62", 3);
		//if (result > 0)
		//	printf("Message %d: Sent to PRU\n", i);

		/* Poll until we receive a message from the PRU and then print it */
		result = read(pollfds[0].fd, readBuf, MAX_BUFFER_SIZE);
		if (result > 0)
			/*printf("\r%02d: %s",i,readBuf);*/
			
			printf("\r%02d Rising: 1:%d, 2:%d, 3:%d, 4:%d, 5:%d, 6:%d, 7:%d, 8:%d, Falling:,1:%d, 2:%d, 3:%d, 4:%d, 5:%d, 6:%d, 7:%d, 8:%d",
				i,
				readBuf[0]-48,readBuf[1]-48,readBuf[2]-48,readBuf[3]-48,readBuf[4]-48,readBuf[5]-48,readBuf[6]-48,readBuf[7]-48,
				readBuf[BIT_COUNT+0]-48,readBuf[BIT_COUNT+1]-48,readBuf[BIT_COUNT+2]-48,readBuf[BIT_COUNT+3]-48,readBuf[BIT_COUNT+4]-48,
				readBuf[BIT_COUNT+5]-48,readBuf[BIT_COUNT+6]-48,readBuf[BIT_COUNT+7]-48);
			
		fflush(stdout);
	}

	/* Received all the messages the example is complete */
	printf("\r\nReceived %d messages, closing %s\n", NUM_MESSAGES, DEVICE_NAME);

	/* Close the rpmsg_pru character device file */
	close(pollfds[0].fd);

	return 0;
}


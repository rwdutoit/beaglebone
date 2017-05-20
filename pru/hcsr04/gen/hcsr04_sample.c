#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/poll.h>

#define MAX_BUFFER_SIZE		512
char readBuf[MAX_BUFFER_SIZE];

#define NUM_MESSAGES		30
#define DEVICE_PRU0		"/dev/rpmsg_pru30"
#define BIT_COUNT		62 + 36

int main(void)
{
	struct pollfd pollfds[1];
	int i;
	int result = 0;

	/* Open the rpmsg_pru character device file */
	pollfds[0].fd = open(DEVICE_PRU0, O_RDWR);

	/*
	 * If the RPMsg channel doesn't exist yet the character device
	 * won't either.
	 * Make sure the PRU firmware is loaded and that the rpmsg_pru
	 * module is inserted.
	 */
	if (pollfds[0].fd < 0) {
		printf("Failed to open %s\n", DEVICE_PRU0);
		return -1;
	}

	/* The RPMsg channel exists and the character device is opened */
	printf("Opened %s, sending %d messages\n\n", DEVICE_PRU0, NUM_MESSAGES);

	for (i = 0; i < NUM_MESSAGES; i++) 
	{
		usleep(1000000);
		/* Send 'hello world!' to the PRU through the RPMsg channel */
		result = write(pollfds[0].fd, "12", 3);
		//if (result > 0)
		//	printf("Message %d: Sent to PRU\n", i);

		/* Poll until we receive a message from the PRU and then print it */
		result = read(pollfds[0].fd, readBuf, MAX_BUFFER_SIZE);
		if (result > 0)
		{
			printf("\r %c[2K %02d: %s",27,i,readBuf);
			fflush(stdout);
		}
	}

	/* Received all the messages the example is complete */
	printf("\r\nReceived %d messages, closing %s\n", NUM_MESSAGES, DEVICE_PRU0);

	/* Close the rpmsg_pru character device file */
	close(pollfds[0].fd);

	return 0;
}


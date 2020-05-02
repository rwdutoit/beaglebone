#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/poll.h>
#include <stdlib.h>
#include <sys/ioctl.h>

#define MAX_BUFFER_SIZE		480
char readBuf[MAX_BUFFER_SIZE];

//#define num_messages		4*4
#define DEVICE_PRU0		"/dev/rpmsg_pru30"

int main(int argc, char *argv[])
{
	struct pollfd pollfds[1];
	int i, num_messages = 0;
	int result = 0;
        char temp[3];
	int adc_val[MAX_BUFFER_SIZE/2];
	int counter = 0;
	int counter_val = 0;

	num_messages = 480;
	if(argc > 1)
	{
		num_messages = atoi(argv[1]);
	}
        temp[0] = ((int)(num_messages/10/10 ) + 48);
        temp[1] = (int)(num_messages % 100 / 10) + 48;
        temp[2] = (int)(num_messages % 10) + 48;

	for (i = 0; i < MAX_BUFFER_SIZE/2; i++)
	{
		adc_val[i] = 0;
	}

	printf("Opening %s\r\n", DEVICE_PRU0);
	pollfds[0].fd = open(DEVICE_PRU0, O_RDWR);

	if (pollfds[0].fd < 0) {
		printf("Failed to open %s\r\n", DEVICE_PRU0);
		return -1;
	}

	char keepRunning = 1;
	while (keepRunning)
	{

		/* The RPMsg channel exists and the character device is opened */
		//printf("Opened %s, sending %d messages\r\n", DEVICE_PRU0, num_messages);
		//printf("Number of  bytes: %s (%d) = %i \r\n",temp, sizeof(temp), num_messages);
		result = write(pollfds[0].fd, temp, sizeof(temp));
		printf("Request sent to PRU: %s\r\n", temp);
		result = read(pollfds[0].fd, readBuf, MAX_BUFFER_SIZE);
		printf("Results from PRU: %d\r\n", result);
		if (result > 0)
		{
			counter = 0;
			counter_val = 0;
			do
			{
				adc_val[counter_val] = (int)readBuf[counter++] | ((int)readBuf[counter++] << 8);
				printf("value %d = %d\r\n", counter_val, adc_val[counter_val]);
				counter_val++;
			}while (counter < MAX_BUFFER_SIZE);
		}
		else
		{
			printf("result = %i", result);
		}
		fflush(stdout);
	}

	printf("\r\nReceived %d messages, closing %s\n", num_messages, DEVICE_PRU0);

	close(pollfds[0].fd);

	return 0;
}


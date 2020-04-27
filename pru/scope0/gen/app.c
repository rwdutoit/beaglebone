#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/poll.h>
#include <stdlib.h>
#include <sys/ioctl.h>

#define MAX_BUFFER_SIZE		512
char readBuf[MAX_BUFFER_SIZE];

//#define num_messages		4*4
#define DEVICE_PRU0		"/dev/rpmsg_pru30"
#define BIT_COUNT		62 + 36

int main(int argc, char *argv[])
{
	struct pollfd pollfds[1];
	int i, counter, num_messages = 0;
	int result = 0;
        char temp[3];

	num_messages = 16;
	if(argc > 1)
	{
		num_messages = atoi(argv[1]);
	}
        temp[0] = ((int)(num_messages/10/10 ) + 48);
        temp[1] = (int)(num_messages % 100 / 10) + 48;
        temp[2] = (int)(num_messages % 10) + 48;

	pollfds[0].fd = open(DEVICE_PRU0, O_RDWR);

	if (pollfds[0].fd < 0) {
		printf("Failed to open %s\n", DEVICE_PRU0);
		return -1;
	}

	/* The RPMsg channel exists and the character device is opened */
	printf("Opened %s, sending %d messages\r\n", DEVICE_PRU0, num_messages);
	printf("Number of  bytes: %s (%d) = %i \r\n",temp, sizeof(temp), num_messages);

	result = write(pollfds[0].fd, temp, sizeof(temp));
	result = read(pollfds[0].fd, readBuf, num_messages);
	if (result > 0)
	{
    		struct winsize w;
    		ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    		printf ("lines %d\n", w.ws_row);
    		printf ("columns %d\n", w.ws_col);
		for(i=0;i < w.ws_col; i++)
		{
			printf("\u2593");
		}

		counter = 0;
		int chan1_count = (int)readBuf[0] | ((int)readBuf[1] << 8);
		int chan2_count = (int)readBuf[2] | ((int)readBuf[3] << 8);
		//printf("counter 1 = %d\r\n", chan1_count );
		//printf("counter 2 = %d\r\n", chan2_count );
		printf("channel 1 (count = %d)\r\n",chan1_count);
		int current_val = 0;
		int prev_val = 0;
		for (i = 4 ; i < (chan1_count + 4) ; i += 4)
		{
			current_val = (int)readBuf[i] | ((int)readBuf[i + 1] << 8) | ((int)readBuf[i + 2] << 16) | ((int)readBuf[i + 3] << 24);
			if( counter > 0)
			{
				printf("	diff - %d\r\n", current_val - prev_val );
			}
			printf("%i = %d\r\n", counter++,  current_val );
			prev_val = current_val;
		}
		counter = 0;
		printf("channel 2 (count = %d)\r\n",chan2_count);
		for (i = (chan1_count + 4) ; i < (chan1_count + chan2_count + 4) ; i += 4)
		{
			printf("%i = %d\r\n", counter++, (int)readBuf[i] | ((int)readBuf[i + 1] << 8) | ((int)readBuf[i + 2] << 16) | ((int)readBuf[i + 3] << 24) );
		}
	}
	else
	{
		printf("result = %i", result);
	}
	fflush(stdout);

	printf("\r\nReceived %d messages, closing %s\n", num_messages, DEVICE_PRU0);

	close(pollfds[0].fd);

	return 0;
}


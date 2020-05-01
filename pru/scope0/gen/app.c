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
	int i, num_messages = 0;
	int result = 0;
        char temp[3];

	num_messages = 240;
	if(argc > 1)
	{
		num_messages = atoi(argv[1]);
	}
        temp[0] = ((int)(num_messages/10/10 ) + 48);
        temp[1] = (int)(num_messages % 100 / 10) + 48;
        temp[2] = (int)(num_messages % 10) + 48;
	volatile int intBuf[num_messages/4];
	volatile int intBuf2[num_messages/4];
	for (i = 0; i < (num_messages/4); i++)
	{
		intBuf[i] = 0;
		intBuf2[i] = 0;
	}

	pollfds[0].fd = open(DEVICE_PRU0, O_RDWR);

	if (pollfds[0].fd < 0) {
		printf("Failed to open %s\n", DEVICE_PRU0);
		return -1;
	}

	char keepRunning = 1;
	while (keepRunning)
	{

		/* The RPMsg channel exists and the character device is opened */
		//printf("Opened %s, sending %d messages\r\n", DEVICE_PRU0, num_messages);
		//printf("Number of  bytes: %s (%d) = %i \r\n",temp, sizeof(temp), num_messages);
		result = write(pollfds[0].fd, temp, sizeof(temp));
		result = read(pollfds[0].fd, readBuf, num_messages);
		if (result > 0)
		{
			int counter1 = 0;
			int chan1_count = (int)readBuf[0] | ((int)readBuf[1] << 8);
			int chan2_count = (int)readBuf[2] | ((int)readBuf[3] << 8);
			//printf("counter 1 = %d\r\n", chan1_count );
			//printf("counter 2 = %d\r\n", chan2_count );
			int cur_val = 0;
			int offset = 8;
			//printf("channel 1 (count = %d)\r\n",chan1_count);
			for (i = offset ; i < (chan1_count + offset) ; i += 4)
			{
				cur_val = (int)readBuf[i] | ((int)readBuf[i + 1] << 8) |
					   	((int)readBuf[i + 2] << 16) | ((int)readBuf[i + 3] << 24);
				intBuf[counter1] = cur_val;
				if( counter1 > 0)
				{
					int diff = (intBuf[counter1] - intBuf[counter1 - 1]);
					//printf("	diff - %d\r\n", diff );
				}
				//printf("%i = %d (%d) \r\n", counter1, intBuf[counter1], cur_val );
				counter1++;
			}
			int counter2 = 0;
			//printf("channel 2 (count = %d)\r\n",chan2_count);
			for (i = (chan1_count + offset) ; i < (chan1_count + chan2_count + offset) ; i += 4)
			{
				cur_val = (int)readBuf[i] | ((int)readBuf[i + 1] << 8) |
						((int)readBuf[i + 2] << 16) | ((int)readBuf[i + 3] << 24);
				intBuf2[counter2] = cur_val;
				//printf("%i = %d (%d) \r\n", counter2, intBuf2[counter2], cur_val );
				if( counter2 > 0)
				{
					int diff = (intBuf2[counter2] - intBuf2[counter2 - 1]);
					//printf("	diff - %d\r\n", diff );
				}
				//printf("%i = %d (%d) \r\n", counter1, intBuf[counter1], cur_val );
				counter2++;
			}
	///////////////////////////
	//plotting
	//////////////////////////
    			struct winsize w;
    			ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
			int col_count = (int)w.ws_col;
    			//printf ("lines %d\n", w.ws_row);
    			//printf ("columns %d\n", w.ws_col);
			int max_time = ( intBuf2[counter2 - 1] > intBuf[counter1 - 1] ) ? intBuf2[counter2 - 1] : intBuf[counter1 -1];
			int min_time = ( intBuf2[0] < intBuf[0] ) ? intBuf2[0] : intBuf[0];
			volatile int time_diff = (max_time - min_time);
			volatile int time_per_col = time_diff /  col_count;
			//printf ("time per col %d = (%d / %d) last counter = %d\r\n", time_per_col, time_diff, col_count, (counter1 - 1) );
			volatile int current_time = 0;
			volatile int up_down = 1;
	////////////
	//Chan1
	//////////
			printf("%c[H%c[J Reading\r\n",27,27);
			printf ("\r\nChannel 1\r\n");
			counter1 = 0;
			for(i = 0; i < col_count; i++)
			{
				current_time = min_time + ( i * time_per_col );
				//printf ("current_time = %d for col = %d\r\n", current_time, i);
				if ( current_time >= intBuf[counter1]  )
				{
					//printf ("Edge detected at %d \r\n" , counter1);
					up_down ^= 1;
					counter1++;
					if(counter1 > chan1_count)
						break;
				}
				if(up_down)
					printf("\u2593");
				else
					printf("_");
			}
	////////////
	//Chan2
	//////////
			printf ("\r\nChannel 2\r\n");
			counter1 = 0;
			for(i = 0; i < col_count; i++)
			{
				current_time = min_time + ( i * time_per_col );
				//printf ("current_time = %d for col = %d\r\n", current_time, i);
				if ( current_time >= intBuf2[counter1]  )
				{
					//printf ("Edge detected at %d \r\n" , counter1);
					up_down ^= 1;
					counter1++;
					if(counter1 > chan2_count)
						break;
				}
				if(up_down)
					printf("\u2593");
				else
					printf("_");
			}
			printf ("\r\nValues %x, %x, %x, %x \r\n", readBuf[4], readBuf[5], readBuf[6], readBuf[7] );


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


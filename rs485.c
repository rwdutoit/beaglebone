#include <sys/ioctl.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <asm/ioctls.h>
#include <errno.h>
#include <termios.h>
 
//Control struct for setting the port in 485 mode
struct rs485_ctrl {
  unsigned short rts_on_send;
  unsigned short rts_after_sent;
  unsigned int  delay_rts_before_send;
  unsigned short enabled;
};
 
//This struct is only needed if we would like to write with ioctl()
struct rs485_wrt {
  unsigned short outc_size;
  unsigned char *outc;
};
 
int main(void)
{
	char texttosend[] = "This is a short text that is about to be sent";
	struct rs485_ctrl ctrl485;
	struct rs485_wrt io485;
	int status;
	int fd;
	char dev[] = "/dev/ttyS3";
	unsigned char buf[100];
	int size;	
	struct termios ti;
	speed_t speed;
 
	//open the port
	fd = open(dev, O_RDWR); // to open as non blocking use "O_RDWR|O_NONBLOCK"
	if (fd < 0) {
		printf("ERROR! Failed to open %s\n", dev);
		return -1;
	}
 
	// Set the port in 485 mode
	ctrl485.rts_on_send = 0;
	ctrl485.rts_after_sent = 1;
	ctrl485.delay_rts_before_send = 0;
	ctrl485.enabled = 1;
	status = ioctl(fd, TIOCSERSETRS485, &ctrl485);
	if (status) {
		printf("ERROR PORT 1! TIOCSERSETRS485 failed %i\r\n", status);
		return -1;
	}
 
	// Set the port in raw mode
	tcgetattr(fd, &ti);
	cfmakeraw(&ti);
	tcsetattr(fd, TCSANOW, &ti);
 
	// Set the port speed
	speed = B9600;
	tcgetattr(fd, &ti);
	cfsetospeed(&ti, speed);
	cfsetispeed(&ti, speed);
	tcsetattr(fd, TCSANOW, &ti);
 
	// Writing with ioctl
	io485.outc_size = strlen(texttosend);
	io485.outc = texttosend;
	status = ioctl(fd, TIOCSERWRRS485, &io485);
	if (status != io485.outc_size) {
		printf("ERROR! TIOCSERWRRS485 failed %i (%s) \r\n", status, dev);
	}
 
	// Writing with write()
	if (write(fd, texttosend, strlen(texttosend)) != strlen(texttosend)) {
		printf("ERROR! write() failed \r\n");
	}
 
	// Read with read()
	size = read(fd, buf, sizeof(buf)-1);
	if (size > 0) {
		printf("Read the following %d byte(s): %s \r\n", size, buf);
	}
	else if (size == 0) { 
		printf("End of file... \r\n");
	}
	else { //some error...
		if (errno == EAGAIN) {// This indicates that no new data is aviable if we opened the file descriptor as non blocking
			printf("No new data...\r\n");
		}
		else {
			printf("ERROR reading %s: %i %m\r\n", dev, errno);
		}
	}
 
return 0;
}

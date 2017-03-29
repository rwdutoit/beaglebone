// rs485tx.c
// Basic example of RS485 half duplex transmission

#include <sys/ioctl.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <asm/ioctls.h>
#include <errno.h>
#include <termios.h>  

#define TIOCSERSETRS485
/* RS485 ioctls: */
#define TIOCGRS485      0x542E
#define TIOCSRS485 0x542F

// Control struct for setting the port in 485 mode
struct rs485_ctrl {
  unsigned short rts_on_send;
  unsigned short rts_after_sent;
  unsigned int  delay_rts_before_send;
  unsigned short enabled;
};
 
// his struct is only needed if we would like to write with ioctl()
struct rs485_wrt {
  unsigned short outc_size;
  unsigned char *outc;
};
 
int main(void) {
  char dev[] = "/dev/ttyS3";
  char buffer_to_send[] = { 0x55, 0x00, 0x55 };
  int  baudrate = B9600;

  struct rs485_ctrl ctrl485;
  int status;
  int fd;
  struct termios ti;
  struct termios ti_prev;
  
  // Open the serial port
  fd = open(dev, O_RDWR|O_NONBLOCK);
  if (fd < 0) {
    printf("ERROR! Failed to open %s\n", dev);
    return -1;
  }
 
  // Set the serial port in 485 mode
  ctrl485.rts_on_send = 0;           // It means that DE is at 3.3 volt on send
  ctrl485.rts_after_sent = 1;        // It means that DE is at 0 volt on send
  ctrl485.delay_rts_before_send = 0; // DE will be active at same time of data
  ctrl485.enabled = 1;
  status = ioctl(fd, TIOCSERSETRS485, &ctrl485);  
  if (status) {
    printf("ERROR PORT 1! TIOCSERSETRS485 failed %i", status);
    return -1;
  }
 
  tcgetattr(fd, &ti_prev);    // Save the previous serial config
  tcgetattr(fd, &ti);         // Read the previous serial config
  cfsetospeed(&ti,baudrate);  // Set the TX baud rate
  cfsetispeed(&ti,baudrate);  // Set the RX baud rate

  cfmakeraw(&ti);
  tcsetattr(fd, TCSANOW, &ti);  // Set the new serial config
 
  // Send buffer_to_send content to RS485
  if (write(fd, buffer_to_send, sizeof(buffer_to_send)) != sizeof(buffer_to_send)) {
    printf("ERROR! write() failed \r\n");
  }

  ti_prev.c_cflag &= ~HUPCL;        // This to release the RTS after close
  tcsetattr(fd, TCSANOW, &ti_prev); // Restore the previous serial config
  close(fd);

  return 0;
}

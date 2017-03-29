#include <linux/serial.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

/* RS485 ioctls: */
#define TIOCGRS485      0x542E
#define TIOCSRS485 0x542F

//This struct is only needed if we would like to write with ioctl()
struct rs485_wrt {
  unsigned short outc_size;
  unsigned char *outc;
};
int main(void) 
{
	struct rs485_wrt io485;
char texttosend[] = "This is a short text that is about to be sent";
char dev[] = "/dev/ttyS4";
int status;
         /* Open your specific device (e.g., /dev/mydevice): */
        int fd = open (dev, O_RDWR);
        if (fd < 0) {
                /* Error handling. See errno. */
        }
 
        struct serial_rs485 rs485conf;

        /* Enable RS485 mode: */
         rs485conf.flags |= SER_RS485_ENABLED;
 
        /* Set logical level for RTS pin equal to 1 when sending: */
        rs485conf.flags |= SER_RS485_RTS_ON_SEND;
        /* or, set logical level for RTS pin equal to 0 when sending: */
       rs485conf.flags &= ~(SER_RS485_RTS_ON_SEND);
 
        /* Set logical level for RTS pin equal to 1 after sending: */
       rs485conf.flags |= SER_RS485_RTS_AFTER_SEND;
        /* or, set logical level for RTS pin equal to 0 after sending: */
        rs485conf.flags &= ~(SER_RS485_RTS_AFTER_SEND);
 
         /* Set rts delay before send, if needed: */
       rs485conf.delay_rts_before_send = 1;

         /* Set rts delay after send, if needed: */
       rs485conf.delay_rts_after_send = 1;
 
        /* Set this flag if you want to receive data even whilst sending data */
       rs485conf.flags |= SER_RS485_RX_DURING_TX;

         if (ioctl (fd, TIOCSRS485, &rs485conf) < 0) {
                /* Error handling. See errno. */
        }
         /* Use read() and write() syscalls here... */
 	

       /* Close the device when finished: */
         if (close (fd) < 0) {
                 /* Error handling. See errno. */
}

return 0;
}

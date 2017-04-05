#include <linux/serial.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>

#define TIOCGRS485 0x542E
#define TIOCSRS485 0x542F

int main(void) {
  struct serial_rs485 rs485conf;
  char port[] = "/dev/ttyO4";
  char data[] = { "Testing" };
  int status, fd, n_bytes;
  rs485conf.flags |= SER_RS485_ENABLED;
  rs485conf.flags |= (SER_RS485_RTS_ON_SEND);
  rs485conf.flags |= SER_RS485_RTS_AFTER_SEND;
  // Open the serial port
  fd = open(port, O_RDWR);
  if (fd < 0) {
    printf("Error opening file %s\n", port);
    return -1;
  }
  status = ioctl(fd, TIOCSRS485, &rs485conf);
  if (status < 0) {
    printf("Error setting ioctl - exit status : %i\n", status);
    printf("                    -       errno : %i\n", errno);
    return -1;
  }
  printf("UART now in RS485 mode!\n");

  n_bytes = write(fd, data, sizeof(data));
  if (n_bytes < sizeof(data)) {
    printf("Only %i of %i bytes written!", n_bytes, sizeof(data));
  }
  close(fd);
  return 0;
}

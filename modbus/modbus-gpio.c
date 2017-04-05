/********************************************************************
 * This file is based on:
 * https://developer.ridgerun.com/wiki/index.php/Gpio-int-test.c
 * It is intended to allow use GPIO pins on beaglebone boards instead
 * of RTS for RS485 communication.
 */

//#include <stdio.h>
//#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>

#include "modbus-gpio.h"


#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h> 
//#include "beaglebone_gpio.h"


/****************************************************************
 * Constants
 ****************************************************************/
 
//#define SYSFS_GPIO_DIR "/sys/class/gpio"
//#define MAX_BUF 64

/*
#define GPIO0_START_ADDR 0x44E07000
#define GPIO0_END_ADDR 0x44E08FFF
#define GPIO0_SIZE (GPIO0_END_ADDR - GPIO0_START_ADDR)
#define GPIO_OE 0x134
#define GPIO_SETDATAOUT 0x194
#define GPIO_CLEARDATAOUT 0x190
#define PIN (1<<7)
*/

    volatile void *gpio_addr = NULL;
    volatile unsigned int *gpio_oe_addr = NULL;
    volatile unsigned int *gpio_setdataout_addr = NULL;
volatile unsigned int *gpio_cleardataout_addr = NULL;
unsigned int reg;
int fd;
/****************************************************************
 * gpio_export
 ****************************************************************/
int gpio_export(unsigned int gpio)
{
   fd = open("/dev/mem", O_RDWR);

/*    printf("Mapping %X - %X (size: %X)\n", GPIO1_START_ADDR,
GPIO1_END_ADDR, GPIO1_SIZE);
*/

    gpio_addr = mmap(0, GPIO0_SIZE, PROT_READ | PROT_WRITE,
MAP_SHARED, fd, GPIO0_START_ADDR);

    gpio_oe_addr = gpio_addr + GPIO_OE;
    gpio_setdataout_addr = gpio_addr + GPIO_SETDATAOUT;
    gpio_cleardataout_addr = gpio_addr + GPIO_CLEARDATAOUT;

if(gpio_addr == MAP_FAILED)
{
        //printf("Unable to map GPIO\n");
close(fd);
        return fd;
}
else
{
    reg = *gpio_oe_addr;
    //printf("GPIO1 configuration: %X\n", reg);
    reg = reg & (0xFFFFFFFF - PIN);
    *gpio_oe_addr = reg;
close(fd);
	return 0;

}
}

/****************************************************************
 * gpio_unexport
 ****************************************************************/
int gpio_unexport(unsigned int gpio)
{
   fd = open("/dev/mem", O_RDWR);

/*    printf("Mapping %X - %X (size: %X)\n", GPIO1_START_ADDR,
GPIO1_END_ADDR, GPIO1_SIZE);
*/

    gpio_addr = mmap(0, GPIO0_SIZE, PROT_READ | PROT_WRITE,
MAP_SHARED, fd, GPIO0_START_ADDR);

    gpio_oe_addr = gpio_addr + GPIO_OE;
    gpio_setdataout_addr = gpio_addr + GPIO_SETDATAOUT;
    gpio_cleardataout_addr = gpio_addr + GPIO_CLEARDATAOUT;

if(gpio_addr == MAP_FAILED)
{
        //printf("Unable to map GPIO\n");
close(fd);
        return fd;
}
else
{
    reg = *gpio_oe_addr;
    //printf("GPIO1 configuration: %X\n", reg);
    reg = reg | PIN;
    *gpio_oe_addr = reg;
close(fd);
	return 0;

}

}

/****************************************************************
 * gpio_set_dir
 ****************************************************************/
int gpio_set_dir(unsigned int gpio, unsigned int out_flag)
{
/*	int fd, len;
	char buf[MAX_BUF];
 
	len = snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR  "/gpio%d/direction", gpio);
	if (len < 0)
	{
		return len;
	}
 
	fd = open(buf, O_WRONLY);
	if (fd < 0) {
		perror("gpio/direction");
		return fd;
	}
 
	if (out_flag)
		write(fd, "out", 4);
	else
		write(fd, "in", 3);
 
	close(fd);
*/
	return 0;
}

/****************************************************************
 * gpio_set_value
 ****************************************************************/
int gpio_set_value(unsigned int gpio, unsigned int value)
{
   fd = open("/dev/mem", O_RDWR);

/*    printf("Mapping %X - %X (size: %X)\n", GPIO1_START_ADDR,
GPIO1_END_ADDR, GPIO1_SIZE);
*/

    gpio_addr = mmap(0, GPIO0_SIZE, PROT_READ | PROT_WRITE,
MAP_SHARED, fd, GPIO0_START_ADDR);

    gpio_oe_addr = gpio_addr + GPIO_OE;
    gpio_setdataout_addr = gpio_addr + GPIO_SETDATAOUT;
    gpio_cleardataout_addr = gpio_addr + GPIO_CLEARDATAOUT;

if(gpio_addr == MAP_FAILED)
{
        //printf("Unable to map GPIO\n");
close(fd);
        return fd;
}
else
{
if(value ==1)
*gpio_setdataout_addr= PIN;
else
*gpio_cleardataout_addr = PIN;
close(fd);
        return 0;

}
}

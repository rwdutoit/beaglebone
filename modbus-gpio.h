#ifndef MODBUS_GPIO_H
#define MODBUS_GPIO_H

/*
#define GPIO0_START_ADDR 0x44E07000
#define GPIO0_END_ADDR 0x44E08FFF
#define GPIO0_SIZE (GPIO1_END_ADDR - GPIO1_START_ADDR)
#define GPIO_OE 0x134
#define GPIO_SETDATAOUT 0x194
#define GPIO_CLEARDATAOUT 0x190
#define PIN (1<<7)
*/

#define GPIO0_START_ADDR 0x44E07000
#define GPIO0_END_ADDR 0x44E08FFF
#define GPIO0_SIZE (GPIO0_END_ADDR - GPIO0_START_ADDR)
#define GPIO_OE 0x134
#define GPIO_SETDATAOUT 0x194
#define GPIO_CLEARDATAOUT 0x190
#define PIN (1<<7)

int gpio_export(unsigned int gpio);
int gpio_unexport(unsigned int gpio);
int gpio_set_dir(unsigned int gpio, unsigned int out_flag);
int gpio_set_value(unsigned int gpio, unsigned int value);

#endif

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <modbus.h>

//#include "test.h"
//#include <modbus-gpio.h>

#define SERVER_ID         17
#define INVALID_SERVER_ID 18

const uint16_t UT_REGISTERS_ADDRESS = 3037;
const uint16_t UT_REGISTERS_NB = 0x1;
//const uint16_t UT_REGISTERS_TAB[] = { 1100 };


modbus_t *ctx;
int twtm_open (const char *device, int slave_address)
{
        ctx = modbus_new_rtu (device, 19200, 'N', 8, 1);
        if (ctx == NULL)
        {
                printf("mb == null");
                return -1;
        }
        if (modbus_set_slave (ctx, slave_address) != 0)
        {
                printf("set_slave == null");
                return -1;
        }
        /*modbus_set_debug (mb, TRUE);*/
        if (modbus_rtu_set_gpio_rts (ctx, 7) != 0)
        {
                printf("gpio not setl");
                return -1;
        }
        if (modbus_connect (ctx) != 0)
        {
                 printf("could not connect mb");
                return -1;
        }
        return 0;
}

int main()
{
    uint8_t *tab_rp_bits;
    uint16_t *tab_rp_registers;
    uint16_t *tab_rp_registers_bad;
int rc;
int nb_points;
const char* device;
device = "/dev/ttyO4";

//unsigned int pin_GPIO_number = 7;
//gpio_export(pin_GPIO_number);
//gpio_set_dir(pin_GPIO_number,1);

twtm_open(device,1);

    /* Allocate and initialize the memory to store the registers */
    nb_points = UT_REGISTERS_NB;
    tab_rp_registers = (uint16_t *) malloc(nb_points * sizeof(uint16_t));
    memset(tab_rp_registers, 0, nb_points * sizeof(uint16_t));


/** HOLDING REGISTERS **/

/* Single register */
    rc = modbus_read_registers(ctx, UT_REGISTERS_ADDRESS,
                               1, tab_rp_registers);
    //printf("2/2 modbus_read_registers: ");
    if (rc != 1) {
        printf("FAILED (nb points %d)\n", rc);
        goto close;
    }

//    if (tab_rp_registers[0] != 0x1234) {
        printf("Read: (%0X)\n",
               tab_rp_registers[0]);
//        goto close;
//    }
    printf("OK\n");
/* End of single register */

close:

    /* Free the memory */
//    free(tab_rp_bits);
//    free(tab_rp_registers);

    /* Close the connection */
    modbus_close(ctx);
modbus_free(ctx);

   return 0;

}


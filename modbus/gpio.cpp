#include<iostream>
#include<fstream>
#include<string>
#include <modbus.h>

using namespace std;

int twtm_open (const char *device, int slave_address)
{
        mb = modbus_new_rtu (device, 9600, 'N', 8, 1);
        if (mb == NULL)
        {
                return -1;
        }
        if (modbus_set_slave (mb, slave_address) != 0)
        {
                return -1;
        }
        /*modbus_set_debug (mb, TRUE);*/
        if (modbus_rtu_set_gpio_rts (mb, 36) != 0)
        {
                return -1;
        }
        if (modbus_connect (mb) != 0)
        {
                return -1;
        }
        return 0;
}
int main(){
   std::fstream fs;
const char* device;
device = "/dev/ttyO4";

twtm_open(device,1);

   fs.open("/sys/class/gpio/gpio7/direction");
   fs << "out";
   fs.close();
   fs.open("/sys/class/gpio/gpio7/value");
   fs << "1";
   fs.close();
   fs.open("/dev/ttyO4");
   fs << "f";
   fs.close();
   fs.open("/sys/class/gpio/gpio7/value");
   fs << "0"; // "0" for off
   fs.close();
   // select whether it is on, off or flash

   return 0;
}

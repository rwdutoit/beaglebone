//List modules (*.ko)
ls /lib/modules/4.4.54-ti-r93
OR
dpkg -S 

//include files: like in c-header files (*.h)
/usr/include/linux/

//dts directory
/opt/source/dtb-4.4-ti/
//Board file
nano /opt/source/dtb-4.4-ti/src/arm/am335x-boneblack-emmc-overlay.dts -c

//Slots - cape
cat /sys/devices/platform/bone_capemgr/slots

//GPIO command line
echo 27 > /sys/class/gpio/export
echo out > /sys/class/gpio/gpio27/direction
echo 1 > /sys/class/gpio/gpio27/value
cat /sys/class/gpio/gpio27/value


//In order to check if an overlay has successfully changed the pins modes of the target pins, utilize the following commands:
cat /sys/kernel/debug/pinctrl/44e10800.pinmux/pins
cat /sys/kernel/debug/pinctrl/44e10800.pinmux/pingroups | less
//provides a list of all “claimed” pins, such as pins that are configured and reserved via overlays.


//Overlays
dtc -O dtb -o /lib/firmware/sample_overlay-00A0.dtbo -b 0 -@ sample_overlay.dts
dtc -O dtb -o ADAFRUIT-SPI0-00A0.dtbo -b 0 -@ ADAFRUIT-SPI0-00A0.dts
    Bit 5: 1 - Input, 0 - Output
    Bit 4: 1 - Pull up, 0 - Pull down
    Bit 3: 1 - Pull disabled, 0 - Pull enabled
    Bit 2 \
    Bit 1 |- Mode
    Bit 0 /

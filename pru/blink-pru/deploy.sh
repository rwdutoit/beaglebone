config-pin P9.29 pruout
sudo cp out/pru-core0.elf /lib/firmware/am335x-pru0-fw
sudo cp out/PRU_Halt.out /lib/firmware/am335x-pru1-fw
sync
rmmod -f pru-rproc
modprobe pru-rproc

echo "set P9.27 and P9.30 to input"
config-pin P9.27 pruin
config-pin P9.30 pruin
echo "copying clk0-pru.out  to  /lib/firmware/am335x-pru0-fw"
cp ./gen/clk0-pru.out /lib/firmware/am335x-pru0-fw
echo "copying  PRU_Halt.out   to  /lib/firmware/am335x-pru1-fw"
cp ./gen/PRU_Halt.out /lib/firmware/am335x-pru1-fw
echo "remove mod pru_rproc"
sudo rmmod -f pru_rproc
ls /dev/rpmsg_pru30
echo "Sync"
sync
sleep 5
echo "start mod pru_rproc"
sudo modprobe pru_rproc
ls /dev/rpmsg_pru30
#echo "write and read command to/from /dev/rpmsg_pru30"
#echo "1" > /dev/rpmsg_pru30
#cat /dev/rpmsg_pru30


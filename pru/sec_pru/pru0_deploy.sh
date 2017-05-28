echo "set P8.11 (output) and P8.15 (input)"
config-pin P8.11 pruout
config-pin P8.15 pruin
echo "copying hcsr04.out  to  /lib/firmware/am335x-pru0-fw"
cp ./gen/hcsr04.out /lib/firmware/am335x-pru0-fw
#echo "copying  PRU_Halt.out   to  /lib/firmware/am335x-pru1-fw"
#cp ./gen/PRU_Halt.out /lib/firmware/am335x-pru1-fw
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
#echo "10" >  /dev/rpmsg_pru30
#cat /dev/rpmsg_pru30


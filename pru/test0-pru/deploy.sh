echo "set P9.27,30 to input"
#config-pin P8.15 pruin #PRU0
#config-pin P8.16 pruin #PRU0
config-pin P9.30 pruin #PRU0
config-pin P9.27 pruin #PRU0
#config-pin P9.28 pruout #PRU0
#config-pin P9.29 pruout #PRU0
echo "copying test0-pru.out  to  /lib/firmware/am335x-pru0-fw"
cp ./gen/test0-pru.out /lib/firmware/am335x-pru0-fw
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
#echo "1" > /dev/rpmsg_pru31
#cat /dev/rpmsg_pru31


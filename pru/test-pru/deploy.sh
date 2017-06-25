echo "set P8.20,21 to input"
#config-pin P8.15 pruin #PRU0
#config-pin P8.16 pruin #PRU0
config-pin P8.20 pruout #PRU1
config-pin P8.21 pruout #PRU1
#config-pin P8.11 pruout #PRU0
#config-pin P8.12 pruout #PRU0
echo "copying test-pru.out  to  /lib/firmware/am335x-pru1-fw"
cp ./gen/test-pru.out /lib/firmware/am335x-pru1-fw
echo "copying  PRU_Halt.out   to  /lib/firmware/am335x-pru0-fw"
cp ./gen/PRU_Halt.out /lib/firmware/am335x-pru0-fw
echo "remove mod pru_rproc"
sudo rmmod -f pru_rproc
ls /dev/rpmsg_pru31
echo "Sync"
sync
sleep 5
echo "start mod pru_rproc"
sudo modprobe pru_rproc
ls /dev/rpmsg_pru31
#echo "write and read command to/from /dev/rpmsg_pru31"
#echo "1" > /dev/rpmsg_pru31
#cat /dev/rpmsg_pru31


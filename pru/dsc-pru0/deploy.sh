echo "set P9.30 and P9.27 to input"
config-pin P9.30 pruin
config-pin P9.27 pruin
echo "copying dsc-pru0.out  to  /lib/firmware/am335x-pru0-fw"
cp ./gen/dsc-pru0.out /lib/firmware/am335x-pru0-fw
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
#echo "write and read command to/from /dev/rpmsg_pru31"
#echo "10" >  /dev/rpmsg_pru31
#cat /dev/rpmsg_pru31


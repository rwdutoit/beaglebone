echo "set P8.20 and P8.21 to input"
config-pin P8.20 pruin
config-pin P8.21 pruin
echo "copying dsc-pru.out  to  /lib/firmware/am335x-pru1-fw"
cp ./gen/dsc-pru.out /lib/firmware/am335x-pru1-fw
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
#echo "10" >  /dev/rpmsg_pru31
#cat /dev/rpmsg_pru31


echo "set P8.45 and P8.46 to input"
config-pin P8.45 pruin
config-pin P8.46 pruin
echo "copying clk-pru.out  to  /lib/firmware/am335x-pru1-fw"
cp ./gen/clk-pru.out /lib/firmware/am335x-pru1-fw
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


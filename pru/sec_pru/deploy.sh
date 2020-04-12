echo "set P9.28,30 to input"
config-pin P9.30 pruin #PRU0
config-pin P9.28 pruin #PRU0

echo "stop pru0"
echo 'stop' > /sys/class/remoteproc/remoteproc1/state 2>/dev/null

echo "copying sec_pru.out  to  /lib/firmware/am335x-pru0-fw"
cp ./gen/sec_pru.out /lib/firmware/am335x-pru0-fw

echo "copying  PRU_Halt.out   to  /lib/firmware/am335x-pru1-fw"
cp ./gen/PRU_Halt.out /lib/firmware/am335x-pru1-fw

echo "set firmware filename"
echo "am335x-pru0-fw" > /sys/class/remoteproc/remoteproc1/firmware 

echo "start mod pru_rproc"
echo 'start' > /sys/class/remoteproc/remoteproc1/state

echo "sending 1 to PRU0"
echo "1" >  /dev/rpmsg_pru30
#echo 'start' > /sys/class/remoteproc/remoteproc1/state
echo "Reading return"
cat /dev/rpmsg_pru30


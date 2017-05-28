echo "sending 1 to PRU0"
echo "20" >  /dev/rpmsg_pru30
echo "Reading return"
cat /dev/rpmsg_pru30


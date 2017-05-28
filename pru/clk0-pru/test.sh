echo "sending 1 to PRU"
echo "1" >  /dev/rpmsg_pru30
echo "Reading return"
cat /dev/rpmsg_pru30


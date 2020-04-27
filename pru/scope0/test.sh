echo "sending 20 to PRU0"
echo "20" >  /dev/rpmsg_pru30
echo "Reading return"
cat /dev/rpmsg_pru30


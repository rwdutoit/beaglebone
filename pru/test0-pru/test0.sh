echo "sending 0 to PRU"
echo "0" >  /dev/rpmsg_pru31
echo "Reading return"
cat /dev/rpmsg_pru31


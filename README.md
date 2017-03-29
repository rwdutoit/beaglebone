# modbus
Libmodbus example program

gcc random-test-server.c -o random-test-server `pkg-config --libs --cflags libmodbus`

gcc test.c -o test `pkg-config --libs --cflags libmodbus`

//build
g++ cgi.cpp -o hello.cgi
//deploy
sudo cp *.cgi /usr/lib/cgi-bin/

dtc -O dtb -o ADAFRUIT-SPI0-00A0.dtbo -b 0 -@ ADAFRUIT-SPI0-00A0.dts

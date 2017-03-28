# modbus
Libmodbus example program

gcc random-test-server.c -o random-test-server `pkg-config --libs --cflags libmodbus`

gcc test.c -o test `pkg-config --libs --cflags libmodbus`

//build
g++ cgi.cpp -o hello.cgi
//deploy
sudo cp *.cgi /usr/lib/cgi-bin/

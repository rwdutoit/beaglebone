# modbus
Libmodbus example program
```
gcc random-test-server.c -o random-test-server `pkg-config --libs --cflags libmodbus`

gcc test.c -o test `pkg-config --libs --cflags libmodbus`

//build
g++ cgi.cpp -o hello.cgi
//deploy
sudo cp *.cgi /usr/lib/cgi-bin/

dtc -O dtb -o ADAFRUIT-SPI0-00A0.dtbo -b 0 -@ ADAFRUIT-SPI0-00A0.dts
```


https://github.com/stephane/libmodbus.git

modify:

modbus-rtu.c

```C/C++
static ssize_t _modbus_rtu_send(modbus_t *ctx, const uint8_t *req, int req_length)
{
+    uint8_t c;
+    /* Make input buffer empty */
+ while (read(ctx->s, &c, 1));
```

modbus.c

```C/C++
int _modbus_receive_msg(modbus_t *ctx, uint8_t *msg, msg_type_t msg_type)
{
    int rc;
    fd_set rset;
    struct timeval tv;
    struct timeval *p_tv;
    int length_to_read;
    int msg_length = 0;
    _step_t step;

    if (ctx->debug) {
        if (msg_type == MSG_INDICATION) {
            printf("Waiting for a indication...\n");
        } else {
            printf("Waiting for a confirmation...\n");
        }
    }

    /* Add a file descriptor to the set */
    FD_ZERO(&rset);
FD_SET(ctx->s, &rset);
+    uint8_t c;
+    /* Make input buffer empty */
+ while (read(ctx->s, &c, 1));
```

```
    # WiFi Example
    auto wlan0
    iface wlan0 inet dhcp
        wpa-ssid "adafruit"
        wpa-psk  "mypassword"
```

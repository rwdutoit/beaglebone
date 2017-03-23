import serial, fcntl, struct

s = serial.Serial(
    port='/dev/ttyO4',
    baudrate=19200,
    parity=serial.PARITY_EVEN,
    stopbits=serial.STOPBITS_ONE,
    bytesize=serial.EIGHTBITS
)

# -- Begin of setup procedure --
# Standard Linux RS485 ioctl:
TIOCSRS485 = 0x542F

RS485_FLAGS = 0x21 # enable RS485 and use GPIO for RE/DE control

RS485_GPIO_PIN = 7

# Pack the config into 8 consecutive unsigned 32-bit values:
serial_rs485 = struct.pack('IIIIIIII',
                           RS485_FLAGS,        # config flags
                           0,                  # delay in us before send
                           0,                  # delay in us after send
                           RS485_GPIO_PIN,     # the pin number used for DE/RE
                           0, 0, 0, 0          # padding - space for more values
                           )

# Apply the ioctl to the open ttyO4 file descriptor:
fd=s.fileno()
fcntl.ioctl(fd, TIOCSRS485, serial_rs485)
# -- End of setup procedure --

# send a simple message
s.write("Hello, world!")

# print all data coming back
while True:
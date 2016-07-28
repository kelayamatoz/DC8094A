import serial
with serial.Serial(port='/dev/cu.usbserial-HRA723C9',baudrate=115200,timeout=0) as ser:
    while 1:
        print "read one line"
        serial_line = ser.read(size=1)
        print serial_line

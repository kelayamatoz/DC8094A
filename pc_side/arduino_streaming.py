import serial
import pymysql.cursors

with serial.Serial(port='/dev/cu.usbserial-HRA723C9',baudrate=230400,timeout=1) as ser:
    while 1:
        serial_line = ser.readline()
        print serial_line

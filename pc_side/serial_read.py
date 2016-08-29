import serial
import sys
import time
iter = 100
with serial.Serial(port='/dev/ttyUSB0',baudrate=230400,timeout=None) as ser:
    ser.flushInput()
    ser.flushOutput()
    start_time = time.time()
    for i in range(0, iter):
        serial_line = ser.readline()
    end_time = time.time()

print 'average ser read time = ', (end_time - start_time) / float(iter)
print serial_line

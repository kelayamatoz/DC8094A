import serial
import pymysql
import sys

if len(sys.argv) == 1:
    print 'usage: python streaming_database.py [test/daq]'
    print 'test is the testing database table, daq is the actual database table'
    sys.exit()

conn = pymysql.connect(host='localhost', port=3306, user='root', password='raspberry', db='mysql')
conn_re = 0
cur = conn.cursor()
tab_string = "USE " + sys.argv[1]
print "database command: ", tab_string
conn_re = conn_re | cur.execute(tab_string)
with serial.Serial(port='/dev/ttyUSB0',baudrate=230400,timeout=1) as ser:
    while 1:
        serial_line = ser.readline()
        print serial_line

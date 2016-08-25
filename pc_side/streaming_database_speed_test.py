import serial
import MySQLdb
import sys
import atexit
import time
def save_exit():
    _conn.commit()
    _conn.close()
    _cur.close()

global _conn
global _cur
_conn = MySQLdb.connect(host='localhost', user='root', passwd='raspberry', db='mysql')
conn_re = 0
_cur = _conn.cursor()
atexit.register(save_exit)
if len(sys.argv) == 1:
    print 'usage: python streaming_database.py [test/daq]'
    print 'test is the testing database table, daq is the actual database table'
    sys.exit()

tab_string = "USE " + sys.argv[1]
_cur.execute(tab_string)
conn_re = conn_re | _cur.execute(tab_string)
if conn_re:
    print 'error: cannot execute ', tab_string
with serial.Serial(port='/dev/ttyUSB0',baudrate=230400,timeout=None) as ser:
    # cleanup the database everytime the program is starting from fresh
    cmd_string = "DROP TABLE data;"
    _cur.execute(cmd_string)
    cmd_string = "CREATE TABLE data (channel_no int, \
            channel_reading float, channel_time double);"
    _cur.execute(cmd_string)
    # disregard the first few readlines as the numbers can be corrupted
    for i in range(0,8):
        ser.readline()
    start = time.time()
    serial_line = ser.readline()
    ser_reading_time = time.time()
    readings = serial_line.split(':')[0:8]
    process_reading_time = time.time()
    reading = readings[0]
    start_inserting_time = time.time()
    cmd_string = "INSERT INTO data VALUES (" + str(1) + ", " \
            + reading + ", " + str(time.time() - start) + ");"
    try:
        _cur.execute(cmd_string)
        end_inserting_time = time.time()
        start_commit_time = end_inserting_time
        _conn.commit()
        end_commit_time = time.time()
    except :
        _conn.rollback()
        pass
    # print stats
    print 'serial reading required time = ', ser_reading_time - start
    print 'split required time = ', process_reading_time - ser_reading_time
    print 'insertion required time = ', end_inserting_time - start_inserting_time
    print 'commit required time = ', end_commit_time - start_commit_time

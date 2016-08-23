import sys
import MySQLdb
import time
import atexit

def save_exit():
    print 'cleaning up database'
    _conn.commit()
    _conn.close()
    _cur.close()

atexit.register(save_exit)
# testing streaming into mysql database
global _conn
_conn = MySQLdb.connect(host='localhost', user='root', passwd='raspberry', db='mysql')
_cur = _conn.cursor()
tab_string = "USE " + sys.argv[1]
print "database command: ", tab_string
_cur.execute(tab_string)
cmd_string = "DROP TABLE data;"
_cur.execute(cmd_string)
cmd_string = "CREATE TABLE data (channel_no int, \
        channel_reading float, channel_time double);"
_cur.execute(cmd_string)

# disregard the first few readlines as the numbers can be corrupted
readings = ['0.0', '1.0', '2.0', '3.0', '4.0', '5.0', '6.0', '7.0']
start = time.time()
# while 1:
cur_time = time.time() - start
for i in range(0,8):
    reading = readings[i]
    channel = i + 1
    cmd_string = "INSERT INTO data VALUES (" + str(channel) + ", " \
            + reading + ", " + str(cur_time) + ");"
    time.sleep(1)
    print cmd_string
    _cur.execute(cmd_string)

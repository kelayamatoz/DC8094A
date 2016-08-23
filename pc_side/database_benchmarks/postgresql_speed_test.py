import psycopg2
import time
import atexit

def save_exit():
   _conn.close()
   _curr.close()

_conn = psycopg2.connect("dbname=test user=postgres")
_curr = conn.cursor()

_curr.execute("DROPT TABLE data;")
_curr.execute("CREATE TABLE data (channel_no int, \
                                channel_reading float, \
                                channel_double double);")

def insertion_speed_test():
    start = time.time()
    for k in range(0, iter):
        _cur.execute('INSERT INTO data VALUES (1, 0.111, '+ str(time.time() - \
            start)+ ');')

    end_time = time.time()
    print 'MySQLdb inserting ', iter, ' entries takes: ', end_time - start, \
                    ' average insertion speed = ', \
                        (end_time - start) / float(iter)

def commit_speed_test(gran_factor):
    start = time.time()
    commit_time_accum = []
    for k in range(0, iter):
        _cur.execute('INSERT INTO data VALUES (1, 0.111, '+ str(time.time() - \
            start)+ ');')
        if k % gran_factor == 0:
            commit_start_time = time.time()
            _conn.commit()
            commit_time_accum.append(time.time() - commit_start_time)

    print 'MySQLdb commiting ', iter, \
          ' entries using granularity factor = ', gran_factor, \
          ' takes on average: ', sum(commit_time_accum) / float(len(commit_time_accum))


import sys
print len(sys.argv)
if len(sys.argv) == 1:
    print 'usage: test test'
    sys.exit()
else:
    arg = sys.argv[1]
    print arg

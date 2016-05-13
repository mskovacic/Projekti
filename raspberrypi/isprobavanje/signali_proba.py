#! /bin/python

import signal, os, time

def handler(signum, frame):
    print 'Ahahaha neces me ovako ugasiti!', signum

# Set the signal handler and a 5-second alarm
#signal.signal(signal.SIGKILL, handler)
signal.signal(signal.SIGINT, handler)
a=time.time()
b=time.time()-a
while b<20:
	b=time.time()-a
	time.sleep(1)
	print 'Preostalo vrijeme {}'.format(20-b)






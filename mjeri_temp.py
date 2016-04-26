#! /usr/bin/python

import time

filename = 'Podaci_o_temperaturi.txt'
sum=0
ticks=0
while True:
	sum+=
	ticks+=1
	time.sleep(5)
	if ticks >= 60:
		t=time.strftime("%c")
		temp=str(sum/ticks)
		with open(filename, 'a') as f:
			f.write("%s %s\n" % (t,temp))
		ticks=0
		sum=0
		#print ("%s %s" % (t,temp))


		
		
				

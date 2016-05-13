#! /usr/bin/python

import time
import os
import re

filename = 'Podaci_o_temperaturi.txt'
sum=0
ticks=0
while True:
	os.system('temp > tmp')
	with open('temp', 'r') as f:
		re.search
		sum+=float(f.readline().split('=\'')[1])
	ticks+=1
	time.sleep(30)
	if ticks >= 10:
		t=time.strftime("%c")
		temp=str(sum/ticks)
		with open(filename, 'a') as f:
			f.write("%s %s\n" % (t,temp))
		ticks=0
		sum=0
		#print ("%s %s" % (t,temp))

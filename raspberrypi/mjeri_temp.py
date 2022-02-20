#! /usr/bin/python

import time
import os

filename = 'Podaci_o_temperaturi.txt'

def get_temp():
	os.system('temp > tmp')
        with open('tmp', 'r') as f:
                temp=float(f.readline().split("=")[1].split("'")[0])
        os.remove('tmp')
	return temp

def write_temp(sum, ticks):
	t=time.strftime("%c")
        temp=str(sum/ticks)
        with open(filename, 'a') as f:
        	f.write("%s %s'C\n" % (t,temp))
	return (0,0)


sum=0
ticks=0
while True:
	sum+=get_temp()
	ticks+=1
	time.sleep(1)
	if ticks >= 10:
		(sum, ticks)=write_temp(sum, ticks)
		
		


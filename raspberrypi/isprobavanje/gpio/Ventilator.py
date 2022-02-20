#!/usr/bin/python
import RPi.GPIO as GPIO
GPIO.setmode(GPIO.BCM)
GPIO.cleanup()
GPIO.setwarnings(False)
GPIO.setup(16,GPIO.OUT)
print "Vent on"
GPIO.output(16,GPIO.HIGH)


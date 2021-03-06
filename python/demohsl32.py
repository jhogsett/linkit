#!/usr/bin/python

import serial 
import time
import sys

s = None

def flush_input():
  s.read(s.inWaiting())

def wait_for_ack():
  while s.inWaiting() == 0:
    pass
  flush_input()

def command(cmd_text):
  s.write((cmd_text + ':').encode())   
  wait_for_ack()

num_leds = 100                          
brightness = 50                             
saturation = 255                            
step = 1                                    
stops = 6                                     
repeats = (num_leds / stops) - 1              
empties = num_leds - ((repeats + 1) * stops)  

def setup():
  global s, stops, repeats, empties, step, brightness
  s = serial.Serial("/dev/ttyS0", 115200) 
  command("::pause:reset:erase")

  if len(sys.argv) > 1:
    stops = int(sys.argv[1])
    repeats = (num_leds / stops) - 1
    empties = num_leds - ((repeats + 1) * stops)

  if len(sys.argv) > 2:
    step = int(sys.argv[2])

  if len(sys.argv) > 3:
    brightness = int(sys.argv[3])

def angle(step, stop):
  return (((360 / stops) * stop) + step) % 360

def loop(): 
  d = 0
  for i in xrange(0, 360, step):

    command(str(d % 2) + ":display")
    d += 1

    for j in range(0, stops):
      command(str(angle(i, j)) + "," + str(saturation) + "," + str(brightness) + ":hslcolor"); command(str(repeats) + ":repeat");
    if empties > 0:
      command("black:" + str(empties - 1) + ":repeat");
    command("continue:flush")
    command("pause")

if __name__ == '__main__':
  setup()
  while True:
    loop()


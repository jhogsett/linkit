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
brightness = 63                             
saturation = 255                            
step = 1                                    
stops = 6                                     
repeats = (num_leds / stops) - 1              
empties = num_leds - ((repeats + 1) * stops)  

def setup():
  global s, stops, repeats, empties, step, brightness
  s = serial.Serial("/dev/ttyS0", 115200) 
  command(":::pau")
  command("rst:era")

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
  for i in xrange(0, 360, step):
    for j in range(0, stops):
      command(str(angle(i, j)) + "," + str(saturation) + "," + str(brightness) + ":hsl"); command(str(repeats) + ":rep");
    if empties > 0:
      command("blk:" + str(empties - 1) + ":rep");
    command("cnt:flu")
    command("pau")

if __name__ == '__main__':
  setup()
  while True:
    loop()


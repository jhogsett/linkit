#!/usr/bin/python

import serial 
import time
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

def setup(): 
  global s 
  s = serial.Serial("/dev/ttyS0", 115200) 
  command("::pause:reset:erase")

brightness = 50
saturation = 255
step = 3

def loop(): 
  for i in xrange(0, 359, step):
    command(str(i) + "," + str(saturation) + "," + str(brightness) + ":hslcolor:continue");
    time.sleep(0.1);
    command("pause");

if __name__ == '__main__': 
  setup() 
  while True: 
    loop()

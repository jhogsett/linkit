#!/usr/bin/python

import serial 
import time
import random

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
  command("::pause:10:level:continue")

def loop():
  command("::pause") 

#  r = random.randrange(0, 3)
#  if r == 0:
#    command("wipe")
#    time.sleep(0.1)
#  elif r == 1:
#    command("fade")
#  else:
#    command("erase")
#    time.sleep(0.3)

  command("random:flood:continue")
  time.sleep(20)

if __name__ == '__main__': 
  setup() 
  while True: 
    loop()


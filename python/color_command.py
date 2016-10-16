#!/usr/bin/python

import serial 
import time
import sys

s = None
inter_command_delay = 0.1

def wait_for_ack():                         
  while s.inWaiting() == 0:            
    pass

def command(cmd):
  s.write((cmd + '|').encode())
  wait_for_ack()

def setup(): 
  global s 
  s = serial.Serial("/dev/ttyS0", 57600) 

  if len(sys.argv) > 1:
    for arg in sys.argv:
      command(arg)
    exit()    

def loop(): 
  cmd = raw_input('command: ')
  command(cmd)

if __name__ == '__main__': 
  setup() 
  while True: 
    loop()

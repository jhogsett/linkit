#!/usr/bin/python

import serial 
import time
import random

s = None

inter_command_delay = 0.05
animation_delay = 0.275

def command(cmd_text):
  s.write((cmd_text + '|').encode())   
  time.sleep(inter_command_delay)  

def setup(): 
  global s 
  s = serial.Serial("/dev/ttyS0", 57600) 
  time.sleep(animation_delay)

def loop():
  command("demo")
  time.sleep(30)

if __name__ == '__main__': 
  setup() 
  while True: 
    loop()

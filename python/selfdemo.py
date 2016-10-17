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

def loop():
  command("demo")
  time.sleep(30)

if __name__ == '__main__': 
  setup() 
  while True: 
    loop()

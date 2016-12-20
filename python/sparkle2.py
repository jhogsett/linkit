#!/usr/bin/python

import serial 
import time
import random
import sys

s = None
play_time = 5

effects = [
  "red:green:2,0:copy:cfade",
  "red:white:2,0:copy:cfade",
  "green:white:2,0:copy:cfade",
  "red:green:white:3,0:copy:cfade",
  "ltblue:white:2,0:copy:cfade",
  "red:pink:2,0:copy:cfade",
  "green:seafoam:2,0:copy:cfade",
  "random:flood:5,0:copy:cfade",
  "random:flood:3,0:copy:cfade",
  "random:flood:2,0:copy:cfade"
]

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
  global s, play_time 
  s = serial.Serial("/dev/ttyS0", 115200) 
  command("::pause:reset")

  if len(sys.argv) > 1:
    command(sys.argv[1])

  if len(sys.argv) > 2:
    play_time = int(sys.argv[2])

r = 0
last_r = 0

def loop():
  global r, last_r

  try:
    while r == last_r: 
      r = random.randrange(0, len(effects))
    last_r = r
    command(effects[r]) 
    time.sleep(play_time)

  except KeyboardInterrupt:                                                                         
    sys.exit("\nExiting...\n")                                                    
  except Exception:                           
    raise  

if __name__ == '__main__': 
  setup() 
  while True: 
    loop()


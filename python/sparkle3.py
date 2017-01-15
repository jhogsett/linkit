#!/usr/bin/python

import serial 
import time
import random
import sys

s = None
play_time = 5

effects = [
  "rnd:flo:5,0:cpy:cfa",
  "rnd:flo:4,0:cpy:cfa",
  "rnd:flo:3,0:cpy:cfa",
  "rnd:flo:2,0:cpy:cfa",
  "red:org:grn:blu:pur:5,0:cpy:cfa",
  "rnd:blk:80:ble:blk:80:ble:blk:80:ble:4,0:cpy:cfa",
  "blk:flo:cfa:wht:flo:cfa:rnd:flo:cfa"
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
  command(":::pau")
  command("rst:clr")

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


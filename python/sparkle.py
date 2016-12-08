#!/usr/bin/python

import serial 
import time
import random
import sys

s = None
play_time = 30

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

def loop():
  try:
    command("::pause") 

    r = random.randrange(0, 3)
    if r == 0:
      command("wipe")
      time.sleep(0.2)
    elif r == 1:
      command("fade")
    else:
      command("erase")
      time.sleep(0.3)

    r = random.randrange(0, 2)
    if r == 0:
      command("1")
    else:
      command("2")

    command("random:flood:continue")
    time.sleep(play_time)
    command("::pause")        

  except KeyboardInterrupt:                                                                         
    sys.exit("\nExiting...\n")                                                    
  except Exception:                           
    raise  

if __name__ == '__main__': 
  setup() 
  while True: 
    loop()


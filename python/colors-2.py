#!/usr/bin/python

import serial 
import time
import random
import sys

s = None
play_time = 5.0

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
  command(":::")
  command("pau:pau")
  command("clr:pau")

  if len(sys.argv) > 1:
    command(sys.argv[1])

  if len(sys.argv) > 2:
    play_time = float(sys.argv[2])

def loop():
  try:
    command("0:dis:flu:rnd:1:cpy:cfa")
    time.sleep(play_time)
    command("1:dis:flu:rnd:1:cpy:cfa")
    time.sleep(play_time)

  except KeyboardInterrupt:                                                                         
    sys.exit("\nExiting...\n")                                                    
  except Exception:                           
    raise  

if __name__ == '__main__': 
  setup() 
  while True: 
    loop()


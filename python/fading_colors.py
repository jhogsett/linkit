#!/usr/bin/python

import serial 
import time
import random
import sys

s = None

num_leds = 100
play_time = 15.0

def flush_input():                        
  s.flushInput()
                                        
def wait_for_ack():                       
  while s.inWaiting() <= 0:               
    pass                                  
  s.read(s.inWaiting())                   

def command(cmd_text):
  s.write((':::' + cmd_text + ':').encode())   
  wait_for_ack()

def clean_up():
  command("reset")
 
def setup(): 
  global s, play_time
  s = serial.Serial("/dev/ttyS0", 115200) 
  flush_input()
  command("pause:reset:pause")

  if len(sys.argv) > 1:
    command(sys.argv[1])

  if len(sys.argv) > 2:
    play_time = int(sys.argv[2])

def loop():
  try:
    command("random:flood:cfade")
    time.sleep(play_time)                                               

  except KeyboardInterrupt:
    sys.exit("\nExiting...\n")
    clean_up()
  except Exception:
    clean_up()
    raise

if __name__ == '__main__': 
  setup() 
  while True: 
    loop()

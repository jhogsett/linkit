#!/usr/bin/python


import serial 
import time
import random

s = None

inter_command_delay = 0.001

def command(cmd_text):                                                                   
  s.write((cmd_text + '|').encode())                                                     
  time.sleep(inter_command_delay)    

def setup(): 
  global s 
  s = serial.Serial("/dev/ttyS0", 57600) 
#  command("fade")
#  time.sleep(2)
  command("erase")
  time.sleep(0.5)

def run(): 
  while True:
    command("pause")

    for x in range(0, 8):
      command("random")
#      command("bright")
#      command("bright")

      command("blinkr")
      command("7|repeat")

#    command("mirror")
    command("continue")
    time.sleep(10.0)

if __name__ == '__main__': 
  setup()
  run() 

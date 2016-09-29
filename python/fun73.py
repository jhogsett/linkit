#!/usr/bin/python


import serial 
import time
import random

s = None

inter_command_delay = 0.0

def command(cmd_text):                                                                   
  s.write((cmd_text + '|').encode())                                                     
  time.sleep(inter_command_delay)    

def setup(): 
  global s 
  s = serial.Serial("/dev/ttyS0", 57600) 
#  command("fade")
#  time.sleep(2)
  command("erase")
  time.sleep(0.1)

blink_map = {
  0 : "blink1",
  1 : "blink2",
  2 : "blink3",
  3 : "blink4",
  4 : "blink5",
  5 : "blink6"
}

def run(): 
  while True:
    command("pause")

    command("random")
    command("random")
    command("blend")
    command("random")
    command("blend")
    command("dim")
    command("dim")
    command("dim")
    command("dim")
    command("flood")

    command("continue")
    time.sleep(5.0)

if __name__ == '__main__': 
  setup()
  run() 

#!/usr/bin/python

import serial 
import time
s = None

inter_command_delay = 0.1
animation_delay = 0.2

def command(cmd_text):
  s.write((cmd_text + '|').encode())   
  time.sleep(inter_command_delay)  

def setup(): 
  global s 
  s = serial.Serial("/dev/ttyS0", 57600) 
  command("pause|0|window|erase")
#  time.sleep(animation_delay)

def loop(): 
  for i in range(0, 64):
    command("1," + str((63-i) + 1) + "|pshifto")
    time.sleep(animation_delay);
    if(i < 63):
      command(str(63-i) + "|window|random")
    else:
      command("random")
    #command("1," + str((63-i) + 1) + "|pshifto")
    #time.sleep(animation_delay);
  command("continue")

if __name__ == '__main__': 
  setup() 
  #while True: 
  loop()

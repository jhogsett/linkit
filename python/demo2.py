#!/usr/bin/python

import serial 
import time
import random

s = None

inter_command_delay = 0.05
animation_delay = 0.275
play_time = 10.0

def command(cmd_text):
  s.write((cmd_text + '|').encode())   
  time.sleep(inter_command_delay)  

def setup(): 
  global s 
  s = serial.Serial("/dev/ttyS0", 57600) 
  command("pause|64|window|erase")
  time.sleep(animation_delay)

size_range_min = 1
size_range_max = 10

num_colors = 12
colors = [ "red", "orange", "yellow", "ltgreen", "green", "seafoam", "cyan", "ltblue", "blue", "purple", "magenta", "pink", "random" ]

def loop():
  osize = random.randrange(size_range_min, size_range_max) + 1
  for i in xrange(0, 63, osize):
    color = colors[random.randrange(0, num_colors)]
    command(str(osize) + "," + str((63-i) + 1) + "|pshifto")
    time.sleep(animation_delay);
    cmd = ""
    if(i < 63):
      cmd = cmd + str(63-i) + "|window|"
    cmd = cmd + color + "|effectr|" + str(osize - 1) + "|repeat|black"    
    command(cmd)

#  command(str(osize) + "," + str((63-i) + 1) + "|pshifto")
  command("64|window|continue")
  time.sleep(play_time)
  command("pause")

if __name__ == '__main__': 
  setup() 
  while True: 
    loop()

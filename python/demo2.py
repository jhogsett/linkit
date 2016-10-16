#!/usr/bin/python

import serial 
import time
import random

s = None

play_time = 5.0

def flush_input():                        
  s.flushInput()
                                        
def wait_for_ack():                       
  while s.inWaiting() <= 0:               
    pass                                  
  s.read(s.inWaiting())                   

def command(cmd_text):
  s.write((cmd_text + '|').encode())   
  wait_for_ack()
 
def setup(): 
  global s 
  s = serial.Serial("/dev/ttyS0", 57600) 
  flush_input()
  command("continue|pause|64|window|erase")

size_range_min = 1
size_range_max = 8

num_colors = 12
colors = [ "red", "orange", "yellow", "ltgreen", "green", "seafoam", "cyan", "ltblue", "blue", "purple", "magenta", "pink", "random" ]

def loop():
  osize = random.randrange(size_range_min, size_range_max) + 1
  for i in xrange(0, 63, osize):
    color = colors[random.randrange(0, num_colors)]
    command(str(osize) + "," + str((63-i) + 1) + "|pshifto")
    cmd = ""
    if(i < 63):
      cmd = cmd + str(63-i) + "|window|"
    cmd = cmd + color + "|effectr|" + str(osize - 1) + "|repeat|black"    
    command(cmd)
  command("64|window|continue")
  time.sleep(play_time)
  command("pause")

if __name__ == '__main__': 
  setup() 
  while True: 
    loop()

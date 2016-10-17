#!/usr/bin/python

import serial 
import time
import random

s = None

num_leds = 64
play_time = 5.0

def flush_input():                        
  s.flushInput()
                                        
def wait_for_ack():                       
  while s.inWaiting() <= 0:               
    pass                                  
  s.read(s.inWaiting())                   

def command(cmd_text):
  s.write((cmd_text + ':').encode())   
  wait_for_ack()
 
def setup(): 
  global s 
  s = serial.Serial("/dev/ttyS0", 115200) 
  flush_input()
  command("continue:pause:64:window:erase")

size_range_min = 1
size_range_max = 8

num_colors = 12
colors = [ "red", "orange", "yellow", "ltgreen", "green", "seafoam", "cyan", "ltblue", "blue", "purple", "magenta", "pink", "random" ]

def loop():
  osize = random.randrange(size_range_min, size_range_max) + 1
  times = int(num_leds / osize)
  times = random.randrange(1, times)
  for i in range(0, times):
    color = colors[random.randrange(0, num_colors)]                   
    command(str(osize) + "," + str(osize * (times - i))  + ":pshifto")          
    cmd = ""                                                          
    cmd = cmd + str(osize * (times - i)) + ":window:"                              
    cmd = cmd + color + ":effectr:" + str(osize - 1) + ":repeat:black"
    command(cmd)               
  command("64:window:continue")                                       
  time.sleep(play_time)                                               
  command("pause")  

if __name__ == '__main__': 
  setup() 
  while True: 
    loop()

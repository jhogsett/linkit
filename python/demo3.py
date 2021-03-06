#!/usr/bin/python

import serial 
import time
import random

s = None

num_leds = 72
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
  command("::pause:64:window:erase")

size_range_min = 1
size_range_max = 8

num_colors = 12
colors = [ "red", "orange", "yellow", "ltgreen", "green", "seafoam", "cyan", "ltblue", "blue", "purple", "magenta", "pink", "random" ]

effects = ['blink1','blink2','blink3','blink4','blink5','blink6']
effect_index = 0

def loop():
  command("0,71:pshifto")
  osize = random.randrange(size_range_min, size_range_max) + 1
  times = int(num_leds / osize) + 0	

  midpoint = times / 2
  effect_index = (midpoint + 1) % 6

  if random.randrange(0, 2) == 0:
    dir = 1
  else:
    dir = -1

  for i in range(0, times):
    effect = effects[effect_index]
    if i < (times / 2):
      effect_index = (effect_index - dir) % 6
    else:
      effect_index = (effect_index + dir) % 6
    color = colors[random.randrange(0, num_colors)]                   
    command(str(osize) + "," + str(osize * (times - i))  + ":pshifto")          
    cmd = ""                                                          
    cmd = cmd + str(osize * (times - i)) + ":window:"                              
    cmd = cmd + color + ":" + effect + ":" + str(osize - 2) + ":repeat:black"
    command(cmd)               
  command("72:window:continue:flush")                                       
  time.sleep(play_time)                                               
  command("pause")  

if __name__ == '__main__': 
  setup() 
  while True: 
    loop()

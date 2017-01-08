#!/usr/bin/python

import serial 
import time
import random

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
  s.write((cmd_text + ':').encode())   
  wait_for_ack()
 
def setup(): 
  global s 
  s = serial.Serial("/dev/ttyS0", 115200) 
  flush_input()
  command(":::pau")
  command("rst:wip")

size_range_min = 2
size_range_max = 13

num_colors = 6
colors = [ "red", "org", "yel", "grn", "blu", "pur" ]
         
effects = ['bli','bl2','bl3','bl4','bl5','bl6']
effect_index = 0

def loop():
  osize = random.randrange(size_range_min, size_range_max) + 0
  times = int(num_leds / osize) + 0	

  midpoint = times / 2
  effect_index = (midpoint + 1) % 6

  if random.randrange(0, 2) == 0:
    dir = 1
  else:
    dir = -1

  for i in range(0, times):
    effect = effects[effect_index]
#    if i < (times / 2):
    effect_index = (effect_index - dir) % 6
#    else:
#      effect_index = (effect_index + dir) % 6
    color = colors[random.randrange(0, num_colors)]              
    command(str(osize) + "," + str(osize * (times - i))  + ":pso")          
    cmd = ""                                                          
    cmd = cmd + str(osize * (times - i)) + ":win:"                              
    cmd = cmd + color + ":" + effect
    if(osize > 1):
      cmd = cmd + ":" + str(osize - 1) + ":rep"
    command(cmd)               
  command("0:win:cnt:flu")                                       
  time.sleep(play_time)                                               
  command("::pau:wip")  

if __name__ == '__main__': 
  setup() 
  while True: 
    loop()

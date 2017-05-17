#!/usr/bin/python

import serial 
import time
import random
import sys

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

size_range_min = 1                                                                                                                    
size_range_max = 8                                                                                                                    
      
def setup(): 
  global s, num_leds, size_range_max, size_range_min, play_time 
  s = serial.Serial("/dev/ttyS0", 115200) 
  flush_input()
  command(":::")
  command("pau:pau")
  command("clr")

  if len(sys.argv) > 1:
    command(sys.argv[1])

  if len(sys.argv) > 2:
    num_leds = int(sys.argv[2])

  if len(sys.argv) > 3:                                                                                                               
    size_range_max = int(sys.argv[3])    

  if len(sys.argv) > 4:
    size_range_min = int(sys.argv[4])

  if len(sys.argv) > 5:
    play_time = float(sys.argv[5])

num_colors = 12
colors = [ "red", "org", "yel", "lgr", "grn", "sea", "cyn", "lbl", "blu", "pur", "mag", "pnk", "rnd" ]

effects = ['bl1','bl2','bl3','bl4','bl5','bl6']
effect_index = 0

def loop():
  #command("0," + str(num_leds-1) + ":pso")
  command("fad")
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
    command(str(osize) + "," + str(osize * (times - i))  + ":pso")          
    cmd = ""                                                          
    cmd = cmd + str(osize * (times - i)) + ":win:"                              
    cmd = cmd + color + ":" + effect + ":" + str(osize - 2) + ":rep:blk"
    command(cmd)               
  command(str(num_leds) + ":win:cnt:flu")                                       
  time.sleep(play_time)                                               
  command("pause")  

if __name__ == '__main__': 
  setup() 
  while True: 
    loop()

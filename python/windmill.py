#!/usr/bin/python

import serial 
import time
import random
import sys

s = None

num_leds = 93
play_time = 0

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
  global s, ticks, play_time 
  s = serial.Serial("/dev/ttyS0", 115200) 
  flush_input()
  choose_colors()
  command(":::pau:clr")

  if len(sys.argv) > 1:
    command(sys.argv[1])

  if len(sys.argv) > 2:                                                                                                                        
    play_time = float(sys.argv[2]) 

  command("6:zon:blk:red:6:rep:blk:grn:6:rep:blk:org:6:rep:blk:blu:6:rep")
  command("5:zon:blk:red:4:rep:blk:grn:4:rep:blk:org:4:rep:blk:blu:4:rep")
  command("4:zon:blk:red:2:rep:blk:grn:2:rep:blk:org:2:rep:blk:blu:2:rep")
  command("3:zon:blk:red:1:rep:blk:grn:1:rep:blk:org:1:rep:blk:blu:1:rep")
  command("2:zon:blk:red:blk:grn:blk:org:blk:blu")
  command("1:zon:pur")

num_colors = 12
colors = [ "red", "orange", "yellow", "ltgreen", "green", "seafoam", "cyan", "ltblue", "blue", "purple", "magenta", "pink", "black", "random" ]

effects = ['blink1','blink2','blink3','blink4','blink5','blink6']
effect_index = 0

chosen_colors = [0,1,2,3,4,5]

def random_color():
  r = random.randrange(0, num_colors)  
  return colors[r]

def choose_colors():
  global chosen_colors
  for i in range(0, 6):
    chosen_colors[i] = random_color()

def shift_colors():
  global chosen_colors
  for i in xrange(5, 0, -1):
    chosen_colors[i] = chosen_colors[i-1]

def clear_colors():
  for j in range(0,6):        
    chosen_colors[j] = "black"

def place_color(zone, color):
  command(str(zone) + ":zone:" + color + ":blink" + str(zone) + ":flood")

def place_colors():
  place_color(6, chosen_colors[0])
  place_color(5, chosen_colors[1])
  place_color(4, chosen_colors[2])
  place_color(3, chosen_colors[3])
  place_color(2, chosen_colors[4])
  place_color(1, chosen_colors[5])

def display():
  place_colors()
  command("flush")

def do_zone(zone):
  command(str(zone) + ":zon:rot")                                                                                                                            

def do_zones():
  for i in range(2, 7):                                                  
    do_zone(i)                                                           
  command("flush") 

global idx                                                                                                                                     
idx = -1                                                                 
                                                                                                                                               
def loop():                              
  global idx                                                                                                                                   
  do_flush = False
  idx = idx + 1                                                          
  if (idx % 15 == 0):                                                                                                                          
    command("6:zon:rot") 
    do_flush = True                                                                                                    
  if (idx % 20 == 0):                                                                                                                          
    command("5:zon:rot")                                  
    do_flush = True                                                          
  if (idx % 30 == 0):                                                                                                                          
    command("4:zon:rot")  
    do_flush = True                                                          
  if (idx % 40 == 0):                                                                                                                          
    command("3:zon:rot")                                                                                                        
    do_flush = True                                                          
  if (idx % 60 == 0):                                                    
    command("2:zon:rot")                                                                                                        
    do_flush = True                                                          
  if (idx % 8 == 0):                                                                                                                           
    command("1:zon:rot")                                       
    do_flush = True                                                          

  if do_flush == True:
    command("flu")

  time.sleep(play_time)

if __name__ == '__main__': 
  setup() 
  while True: 
    loop()

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

  command("6:zon:red:7:rep:yel:7:rep:pur:7:rep:blu:7:rep")
  command("5:zon:red:5:rep:yel:5:rep:pur:5:rep:blu:5:rep")                                                                                                                 
  command("4:zon:red:3:rep:yel:3:rep:pur:3:rep:blu:3:rep")                                                                                                                 
  command("3:zon:red:2:rep:yel:2:rep:pur:2:rep:blu:2:rep")                                                                                                                 
  command("2:zon:red:1:rep:yel:1:rep:pur:1:rep:blu:1:rep")                                                                                                                 

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
    chosen_colors[j] = "blk"

def place_color(zone, color):
  command(str(zone) + ":zon:" + color + ":bli" + str(zone) + ":flo")

def place_colors():
  place_color(6, chosen_colors[0])
  place_color(5, chosen_colors[1])
  place_color(4, chosen_colors[2])
  place_color(3, chosen_colors[3])
  place_color(2, chosen_colors[4])
  place_color(1, chosen_colors[5])

def display():
  place_colors()
  command("flu")

global idx
idx = -1

def do_zone(zone, reverse):
  command(str(reverse) + ":rev:" + str(zone) + ":zon:rot")                                                                                                                            

def loop():
  do_zone(2, 0)
  do_zone(3, 1)
  do_zone(4, 0)
  do_zone(5, 1)
  do_zone(6, 0)
  #do_zone(9, 1)
  #do_zone(0, 0)
  command("flush")

if __name__ == '__main__': 
  setup() 
  while True: 
    loop()

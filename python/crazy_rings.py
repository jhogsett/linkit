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
  command("5:zon:pnk:5:rep:org:5:rep:cyn:5:rep:mag:5:rep")                                                                                                                 
  command("4:zon:lbl:3:rep:red:3:rep:yel:3:rep:grn:3:rep")                                                                                                                 
  command("3:zon:sea:2:rep:red:2:rep:lgr:2:rep:cyn:2:rep")                                                                                                                 
  command("2:zon:yel:1:rep:red:1:rep:pur:1:rep:grn:1:rep")                                                                                                                 
  command("1:zon:rnd:sta")

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
  command("flu")

if __name__ == '__main__': 
  setup() 
  while True: 
    loop()

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
  command(":::pause:reset:erase")

  if len(sys.argv) > 1:
    command(sys.argv[1])

  if len(sys.argv) > 2:                                                                                                                        
    play_time = float(sys.argv[2]) 

  command("6:zone:red:7:repeat:green:7:repeat:orange:7:repeat:blue:7:repeat")
  command("5:zone:red:5:repeat:green:5:repeat:orange:5:repeat:blue:5:repeat")                                                                                                                 
  command("4:zone:red:3:repeat:green:3:repeat:orange:3:repeat:blue:3:repeat")                                                                                                                 
  command("3:zone:red:2:repeat:green:2:repeat:orange:2:repeat:blue:2:repeat")                                                                                                                 
  command("2:zone:red:1:repeat:green:1:repeat:orange:1:repeat:blue:1:repeat")                                                                                                                 

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

def do_zone(zone, count):
  command(str(zone) + ":zone")                                                                                                                            
  for i in range(0, count):                                                                                                                        
    command("rotate:flush")  

def loop():
  for i in range(0, 5):
    do_zone(2, 8)
  for i in range(0, 5):                                                                                                                        
    do_zone(3, 12)                                                                                                                              
  for i in range(0, 5):                                                                                                                        
    do_zone(4, 16)                                                                                                                              
  for i in range(0, 5):                                                                                                                        
    do_zone(5, 24)                                                                                                                              
  for i in range(0, 5):                                                                                                                        
    do_zone(6, 32)                                                                                                                              

if __name__ == '__main__': 
  setup() 
  while True: 
    loop()

#!/usr/bin/python

import serial 
import time
import random
import sys

s = None

num_leds = 93
play_time = 1

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

  command("6:zone:black:red:15:repeat:green:15:repeat")
  command("5:zone:black:red:11:repeat:green:11:repeat")                                                                                                                 
  command("4:zone:black:red:7:repeat:green:7:repeat")                                                                                                                 
  command("3:zone:black:red:5:repeat:green:5:repeat")                                                                                                                 
  command("2:zone:black:red:3:repeat:green:3:repeat")                                                                                                                 

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

def loop():
  command("2:zone")
  for i in range(0, 8):
    command("rotate:flush")
  command("3:zone")
  for i in range(0, 12):
    command("rotate:flush")
  command("4:zone")                                                      
  for i in range(0, 16):                                                 
    command("rotate:flush") 
  command("5:zone")                                                      
  for i in range(0, 24):                                                 
    command("rotate:flush") 
  command("6:zone")                                                      
  for i in range(0, 32):                                                 
    command("rotate:flush") 

  time.sleep(play_time)

if __name__ == '__main__': 
  setup() 
  while True: 
    loop()

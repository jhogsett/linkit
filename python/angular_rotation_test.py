#!/usr/bin/python

import serial 
import time
import random
import sys

s = None

num_leds = 93
ticks = 210

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
  global s, ticks 
  s = serial.Serial("/dev/ttyS0", 115200) 
  flush_input()
  choose_colors()
  command(":::pause:reset:erase")

  if len(sys.argv) > 1:
    command(sys.argv[1])

  if len(sys.argv) > 2:                                                                                                                        
    ticks = int(sys.argv[2]) 

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
  command(str(zone) + ":zone:" + color + ":flood")

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
  global idx
#  time.sleep((60.0 / 480.0) * 0.1)
  idx = idx + 1
  if (idx % 15 == 0):
    command("6:zone:black:flush")
  if (idx % 20 == 0):
    command("5:zone:black:flush")
  if (idx % 30 == 0):
    command("4:zone:black:flush")
  if (idx % 40 == 0):
    command("3:zone:black:flush")
  if (idx % 60 == 0):
    command("2:zone:black:flush")
  if (idx % 8 == 0):
    command("1:zone:random:flush") 
  if (idx % ticks == 0):                                                                                                                         
#    idx = 0                                                      
    command("6:zone:red")                                                                                                                
    command("5:zone:orange")                                 
    command("4:zone:green")                  
    command("3:zone:blue")                  
    command("2:zone:purple")                
    command("flush")                

#  chosen_colors[0] = "red:blink5"
#  chosen_colors[1] = "orange:blink4"     
#  chosen_colors[2] = "green:blink3"     
#  chosen_colors[3] = "blue:blink2"     
#  chosen_colors[4] = "purple:blink1"     
#  chosen_colors[5] = "purple:blink1"     
#  display()
#  exit()  

if __name__ == '__main__': 
  setup() 
  while True: 
    loop()

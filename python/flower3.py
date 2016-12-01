#!/usr/bin/python

import serial 
import time
import random

s = None

num_leds = 93
play_time = 0.0

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
  choose_colors()
  command("::pause:reset:erase:4:level")

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

def display():
  command("::pause")                      
  command(chosen_colors[0] + ":31:repeat")
  command(chosen_colors[1] + ":23:repeat")
  command(chosen_colors[2] + ":15:repeat")
  command(chosen_colors[3] + ":11:repeat")
  command(chosen_colors[4] + ":7:repeat") 
  command(chosen_colors[5])              
  command("continue:flush")
#  time.sleep(play_time)  

def loop():
  color = random_color()

  for i in range(0, 6):
    clear_colors()                        
    chosen_colors[5-i] = color 
    display()

  for i in range(1, 5):                     
    clear_colors()                        
    chosen_colors[i] = color  
    display()

if __name__ == '__main__': 
  setup() 
  while True: 
    loop()

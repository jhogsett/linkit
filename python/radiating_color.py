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
  command(":::pau")
  command("rst:clr")

num_colors = 12
colors = [ "red", "org", "yel", "lgr", "grn", "sea", "cyn", "lbl", "blu", "pur", "mag", "pnk", "blk", "rnd" ]

effects = ['bl1','bl2','bl3','bl4','bl5','bl6']
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

def place_color(window, color):          
  command(str(window) + ":win:" + color + ":flo")
                                                      
def place_colors():                                   
  place_color(93, chosen_colors[0])                   
  place_color(61, chosen_colors[1])                   
  place_color(37, chosen_colors[2])                   
  place_color(21, chosen_colors[3])                   
  place_color(9, chosen_colors[4])                    
  place_color(1, chosen_colors[5])     

def loop():
  color = random_color()

  for i in range(0, 6):
    for j in range(0,6):
      chosen_colors[j] = "blk"
    chosen_colors[5-i] = color 
    place_colors()
    command("flu")                                       

if __name__ == '__main__': 
  setup() 
  while True: 
    loop()

#!/usr/bin/python

import serial 
import time
import random
import sys

s = None

num_leds = 64
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
  global s, ticks, play_time 
  s = serial.Serial("/dev/ttyS0", 115200) 
  flush_input()
  choose_colors()
  command(":::")
  command("pau:pau")
  command("clr:pau")

  if len(sys.argv) > 1:
    command(sys.argv[1])

  if len(sys.argv) > 2:                                                                                                                        
    play_time = float(sys.argv[2]) 

  command("1:zon:grn:4:rep:blk:4:rep")
  command("2:zon:red:12:rep")
  command("3:zon:blu:12:rep")
  command("1:rev:13:rot:0:rev")
  command("4:zon:grn:4:rep")

num_colors = 6
colors = [ "red", "org", "yel", "grn", "blu", "pur" ]

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
  command(str(zone) + ":zone:rotate")                                                                                                                            

def do_zones():
  for i in range(2, 7):                                                  
    do_zone(i)                                                           
  command("flush") 

global idx, color, rotation_count                                                                                                                                     
idx = -1         
color = 'pur'                                                        
rotation_count = 0                 
color_index = 5
                                                                                                                              
def loop():                              
  command("1:rev:1:zon:rot")                                                    
  command("0:rev:2:zon:rot")                                                   
  command("1:rev:3:zon:rot")                                                   
  command("0:rev:4:zon:rot")
  command("flu")
  
  time.sleep(play_time)

if __name__ == '__main__': 
  setup() 
  while True: 
    loop()

#!/usr/bin/python

import serial 
import time
import random
import sys

s = None

num_leds = 93
ticks = 96

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
  command(":::pau:clr")
  command("6:zon:red")                                                                                                                                                                                                                                                        
  command("5:zon:org")                                                                                                                                                                                                                                                        
  command("4:zon:yel")                                                                                                                                                                                                                                                        
  command("3:zon:grn")                                                                                                                                                                                                                                                        
  command("2:zon:blu")   
  command("1:zon:pur")                                                                                                                                                                                                                                                          

  if len(sys.argv) > 1:
    command(sys.argv[1])

  if len(sys.argv) > 2:                                                                                                                        
    ticks = int(sys.argv[2]) 

num_colors = 12
colors = [ "red", "org", "yel", "lgr", "grn", "sea", "cyn", "lbl", "blu", "pur", "mag", "pnk", "blk", "rnd" ]

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
  idx = idx + 1
  do_flush = False
  if (idx % 3 == 0):
    command("6:zon:rot")
    do_flush = True
  if (idx % 4 == 0):
    command("5:zon:rot")
    do_flush = True               
  if (idx % 6 == 0):
    command("4:zon:rot")
    do_flush = True               
  if (idx % 8 == 0):
    command("3:zon:rot")
    do_flush = True               
  if (idx % 12 == 0):
    command("2:zon:rot")
    do_flush = True               
#  if (idx % ticks == 0):                                                                                                                         
#    command("6:zon:red")                                                                                                                
#    command("5:zon:org")                                 
#    command("4:zon:yel")                  
#    command("3:zon:grn")                  
#    command("2:zon:blu")                
#    do_flush = True               

  if do_flush == True:
    command("flu")                

if __name__ == '__main__': 
  setup() 
  while True: 
    loop()

#!/usr/bin/python

import serial 
import time
import random
import sys

s = None

num_leds = 93
play_time = 0
times = 3

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
  global s, ticks, times 
  s = serial.Serial("/dev/ttyS0", 115200) 
  flush_input()
  choose_colors()
  command(":::pau")
  command("rst:clr")

  if len(sys.argv) > 1:
    command(sys.argv[1])

  if len(sys.argv) > 2:                                                                                                                        
    times = int(sys.argv[2]) 

  command("6:zon:red:red:blk:blk:yel:yel:blk:blk:grn:grn:blk:blk:sea:sea:blk:blk:cyn:cyn:blk:blk:blu:blu:blk:blk:pur:pur:blk:blk:pnk:pnk:blk:blk")
  command("5:zon:red:red:blk:yel:yel:blk:grn:grn:blk:sea:sea:blk:cyn:cyn:blk:blu:blu:blk:pur:pur:blk:pnk:pnk:blk")
  command("4:zon:red:blk:yel:blk:grn:blk:sea:blk:cyn:blk:blu:blk:pur:blk:pnk:blk")
  command("3:zon:blk:red:yel:blk:grn:sea:blk:cyn:blu:blk:pur:pnk:blk")
  command("2:zon:red:yel:grn:sea:cyn:blu:pur:pnk")
  command("1:zon:wht")

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
  command("flu")

def do_zone(zone):
  command(str(zone) + ":zon:rot")                                                                                                                            

def do_zones():
  for i in range(2, 7):                                                  
    do_zone(i)                                                           
  command("flu") 

global idx                                                                                                                                     
idx = -1                                                                 

def do_zone(zone):
  command(str(zone) + ":zon:rot")                                                                                                                            

def linear_rotate():                                                         
  for i in range(2, 7):                                                                                                                        
    do_zone(i)                                                                                                                              
  command("flu")
  time.sleep(0.03)

def angular_rotate():
  global idx                                                                                                                                   
  do_flush = False                                                                                                                             
  idx = idx + 1                                                                                                                                
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
                                         
  if do_flush == True:                                                   
    command("flu")                                                     
                                                                                                                                               
def loop():                              
  for i in range(0,2):
    linear_rotate()

  while True:
    for i in range(0,96 * times):
      angular_rotate()

    command("1:rev")
    for i in range(0,4):                                                   
      linear_rotate()                                                      

    for i in range(0,96 * times):            
      angular_rotate()  

    command("0:rev")                                                       
    for i in range(0,4):                                                                                                                         
      linear_rotate()                                                                                                                            


if __name__ == '__main__': 
  setup() 
  while True: 
    loop()

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
  #choose_colors()
  command(":::pau")
  command("rst:era")

  if len(sys.argv) > 1:
    command(sys.argv[1])

  if len(sys.argv) > 2:                                                                                                                        
    play_time = float(sys.argv[2]) 

  command("6:zone:red:16:repeat")
  command("5:zone:yellow:12:repeat")                                                                                                                        
  command("4:zone:green:8:repeat")                                                                                                                        
  command("3:zone:blue:6:repeat")                                                                                                                        
  command("2:zone:purple:4:repeat")                                                                                                                        
  command("1:zone:purple")                                                                                                                        

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

def do_zone(zone):
  command(str(zone) + ":zon:rot")                                                                                                                            

gear1 = 0
gear2 = 0
gear3 = 0
gear4 = 0
gear5 = 0

def loop():
  global gear1, gear2, gear3, gear4, gear5

  command("flu")
  time.sleep(play_time)

  gear1 += 1
  do_zone(6)

  if gear1 % 32 == 0:
    gear2 += 1
    do_zone(5)

    if gear2 % 24 == 0:
      gear3 += 1
      do_zone(4)

      if gear3 % 16 == 0:
        gear4 += 1
        do_zone(3)

        if gear4 % 12 == 0:
          gear5 += 1
          do_zone(2)

if __name__ == '__main__': 
  setup() 
  while True: 
    loop()

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
  command(":::pau:clr")

  if len(sys.argv) > 1:
    command(sys.argv[1])

  if len(sys.argv) > 2:                                                                                                                        
    play_time = float(sys.argv[2]) 

  command("6:zon:red:1:rep:8:cpy")
  command("5:zon:org:brt:6:cpy")                                                                                                                        
  command("4:zon:grn:brt:4:cpy")                                                                                                                        
  command("3:zon:blu:brt:3:cpy")                                                                                                                        
  command("2:zon:pur:brt:2:cpy")                                                                                                                        
  command("1:zon:dgr")                                                                                                                        

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

def rotate_zone(zone):
  command(str(zone) + ":zon:rot")                                                                                                                            

def insert_zone(zone, color, copy):
  command(str(zone) + ":zon:" + color + ":" + str(copy) + ":cpy")

def erase_zone(zone, color, copy):
  command(str(zone) + ":zon:era:" + color + ":brt:" + str(copy) + ":cpy")

gear1 = 0
gear2 = 0
gear3 = 0
gear4 = 0
gear5 = 0
gear6 = 0

def loop():
  global gear1, gear2, gear3, gear4, gear5, gear6

  time.sleep(play_time)
  command("9:zon:rot:flu")

  gear1 += 1
  rotate_zone(6)

  if gear1 % 8 == 0:
    gear2 += 1
    insert_zone(5, 'org', 6)

    if gear2 % 6 == 0:
      insert_zone(4, 'grn', 4)                                            
      gear3 += 1
      erase_zone(5, 'org', 6)

      if gear3 % 4 == 0:
        insert_zone(3, 'blu', 3)                                                                                                                 
        gear4 +=1
        erase_zone(4, 'grn', 4)

        if gear4 % 3 == 0:
          insert_zone(2, 'pur', 2)                                       
          gear5 += 1
          erase_zone(3, 'blu', 3)

          if gear5 % 2 == 0:
            gear6 += 1
            erase_zone(2, 'pur', 2)

if __name__ == '__main__': 
  setup() 
  while True: 
    try:
      loop()
    except KeyboardInterrupt:
      sys.exit("\nExiting...\n")
    except Exception:
      raise

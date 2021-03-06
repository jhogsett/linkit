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

  command("6:zon:pur")
  command("5:zon:blu")                                                                                                                        
  command("4:zon:grn")                                                                                                                        
  command("3:zon:org")                                                                                                                        
  command("2:zon:red")                                                                                                                        
  command("1:zon:red")                                                                                                                        

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
  command(str(zone) + ":zon:" + color + ":flood")

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

def insert_zone(zone, color):
  command(str(zone) + ":zon:" + color)

def erase_zone(zone, color):
  command(str(zone) + ":zon:era:" + color)

gear1 = 0
gear2 = 0
gear3 = 0
gear4 = 0
gear5 = 0
gear6 = 0

def loop():
  global gear1, gear2, gear3, gear4, gear5

  command("flu")
  time.sleep(play_time)

  gear1 += 1
#  insert_zone(2, 'red')
  rotate_zone(2)

  if gear1 % 8 == 0:
#    erase_zone(2, 'red')
    gear2 += 1
    insert_zone(3, 'org')

    if gear2 % 12 == 0:
      erase_zone(3, 'org')
      gear3 += 1
      insert_zone(4, 'grn')

      if gear3 % 16 == 0:
        erase_zone(4, 'grn')
        gear4 += 1
        insert_zone(5, 'blu')

        if gear4 % 24 == 0:
          erase_zone(5, 'blu')
          gear5 += 1
          insert_zone(6, 'pur')

          if gear5 % 32 == 0:
            erase_zone(6, 'pur')

if __name__ == '__main__': 
  setup() 
  while True: 
    try:
      loop()
    except KeyboardInterrupt:
      sys.exit("\nExiting...\n")
    except Exception:
      raise

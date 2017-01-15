#!/usr/bin/python

import serial 
import time
import random
import sys

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
  global s, ticks, play_time 
  s = serial.Serial("/dev/ttyS0", 115200) 
  flush_input()
  #choose_colors()
  command(":::pau:clr")

  if len(sys.argv) > 1:
    command(sys.argv[1])

  if len(sys.argv) > 2:                                                                                                                        
    play_time = float(sys.argv[2]) 

  command("6:zon:mag")
  command("5:zon:red")                                                                                                                        
  command("4:zon:blu")                                                                                                                        
  command("3:zon:grn")                                                                                                                        
  command("2:zon:gry")                                                                                                                        
  command("1:zon:org:brt:brt:brt")                                                                                                                        

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
    chosen_colors[j] = "blk"

def place_color(zone, color):
  command(str(zone) + ":zon:" + color + ":bli" + str(zone) + ":flo")

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
  time.sleep(play_time)
  gear1 += 1
  do_flush = False
  
  if gear1 % 12 == 0:
    do_zone(2)
    do_flush = True

  if gear1 % 31 == 0:
    do_zone(3)
    do_flush = True                                                 

  if gear1 % 51 == 0:
    do_zone(4)
    do_flush = True                                                 

  if gear1 % 94 == 0:
    do_zone(5)
    do_flush = True                                                 

  if gear1 % 595 == 0:
    do_zone(6)
    do_flush = True                                                 

  if do_flush == True:
    command("flu")

if __name__ == '__main__': 
  setup() 
  while True: 
    loop()

#http://nssdc.gsfc.nasa.gov/planetary/factsheet/planet_table_ratio.html
#mercury = 240 / 48 / 24 / 12
#venus = 615 / 124 / 62 / 31
#earth = 1000 / 200 / 100 / 51
#mars = 1880 / 376 / 188 / 94
#jupiter = 11900 / 2380 / 1190 / 595


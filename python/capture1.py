#!/usr/bin/python

import serial                        
import time                          
import random                                                  
import sys 

from subprocess import call
import os,sys              
import Image 

s = None                                                                                                                
                                                                                                                        
num_leds = 72                                                                                                           
play_time = 5.0                                                                                                         
                                                                                                                        
def flush_input():                                                                                                      
  s.flushInput()                       
                                       
def wait_for_ack():                    
  while s.inWaiting() <= 0:                                    
    pass                                                       
  s.read(s.inWaiting())                                        
                                                               
def command(cmd_text):                                         
  s.write((cmd_text + ':').encode())                           
  wait_for_ack()                                               
            
w, h = 16, 16;
matrix = [[0 for x in range(w)] for y in range(h)]
matrix[0]  = [-1, -1, -1, -1, -1,  5, 6, 7, 8, 9, 10, -1, -1, -1, -1, -1]
matrix[1]  = [-1, -1, -1, -1, 11, 10, 9, 8, 7, 6,  5,  4, -1, -1, -1, -1]
matrix[2]  = [-1, -1,  2,  3,  4,  5, 6, 7, 8, 9, 10, 11, 12, 13, -1, -1]
matrix[3]  = [-1, -1, 13, 12, 11, 10, 9, 8, 7, 6,  5,  4,  3,  2, -1, -1]
matrix[4]  = [-1,  1,  2,  3,  4,  5, 6, 7, 8, 9, 10, 11, 12, 13, 14, -1]
matrix[5]  = [15, 14, 13, 12, 11, 10, 9, 8, 7, 6,  5,  4,  3,  2,  1,  0]
matrix[6]  = [ 0,  1,  2,  3,  4,  5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15]
matrix[7]  = [15, 14, 13, 12, 11, 10, 9, 8, 7, 6,  5,  4,  3,  2,  1,  0]
matrix[8]  = [ 0,  1,  2,  3,  4,  5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15]
matrix[9]  = [15, 14, 13, 12, 11, 10, 9, 8, 7, 6,  5,  4,  3,  2,  1,  0]
matrix[10] = [ 0,  1,  2,  3,  4,  5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15]
matrix[11] = [-1, 14, 13, 12, 11, 10, 9, 8, 7, 6,  5,  4,  3,  2,  1, -1]
matrix[12] = [-1, -1,  2,  3,  4,  5, 6, 7, 8, 9, 10, 11, 12, 13, -1, -1]
matrix[13] = [-1, -1, 13, 12, 11, 10, 9, 8, 7, 6,  5,  4,  3,  2, -1, -1]
matrix[14] = [-1, -1, -1, -1,  4,  5, 6, 7, 8, 9, 10, 11, -1, -1, -1, -1]
matrix[15] = [-1, -1, -1, -1, -1, 10, 9, 8, 7, 6,  5, -1, -1, -1, -1, -1]

#matrix[0]  = [-1, -1, -1, -1, -1, 5, 6, 7, 8, 9, 10, -1, -1, -1, -1, -1]
#matrix[1]  = [-1, -1, -1, -1,  4, 5, 6, 7, 8, 9, 10, 11, -1, -1, -1, -1]
#matrix[2]  = [-1, -1,  2,  3,  4, 5, 6, 7, 8, 9, 10, 11, 12, 13, -1, -1]
#matrix[3]  = [-1, -1,  2,  3,  4, 5, 6, 7, 8, 9, 10, 11, 12, 13, -1, -1]
#matrix[4]  = [-1,  1,  2,  3,  4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, -1]
#matrix[5]  = [ 0,  1,  2,  3,  4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15]
#matrix[6]  = [ 0,  1,  2,  3,  4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15]
#matrix[7]  = [ 0,  1,  2,  3,  4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15]
#matrix[8]  = [ 0,  1,  2,  3,  4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15]
#matrix[9]  = [ 0,  1,  2,  3,  4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15]
#matrix[10] = [ 0,  1,  2,  3,  4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15]
#matrix[11] = [-1,  1,  2,  3,  4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, -1]
#matrix[12] = [-1, -1,  2,  3,  4, 5, 6, 7, 8, 9, 10, 11, 12, 13, -1, -1]
#matrix[13] = [-1, -1,  2,  3,  4, 5, 6, 7, 8, 9, 10, 11, 12, 13, -1, -1]
#matrix[14] = [-1, -1, -1, -1,  4, 5, 6, 7, 8, 9, 10, 11, -1, -1, -1, -1]
#matrix[15] = [-1, -1, -1, -1, -1, 5, 6, 7, 8, 9, 10, -1, -1, -1, -1, -1]

def grab_image():                                                                        
  call(["ffmpeg", "-y", "-video_size", "640x480", "-i", "/dev/video0", "-vframes", "2", "/root/dev/linkit/python/test%0d.png"])

def convert_image():                                                                                                                        
  width = 32
  height = 24
  size = width, height
  gif = Image.open("/root/dev/linkit/python/test2.png")
  gif.thumbnail(size, Image.ANTIALIAS)
  gif.save("/root/dev/linkit/python/test2-tinythumb.png")     
  rgb = gif.convert("RGB") 
  return rgb      
      
def display_image(rgb):
  xmargin = 8
  ymargin = 4

#  minr = 255
#  ming = 255
#  minb = 255
  min = 255
  for y in range(0, 16):                                                
    for x in range(0, 16):                                              
      px = matrix[y][x]                                                 
      py = y                                                                                                              
      if px != -1 and py != -1:                                                                                           
        xx = px + xmargin                                               
        yy = py + ymargin                                               
        r, g, b = rgb.getpixel((xx,yy))                                 
        if r < min:
          min = r
        if g < min:
          min = g
        if g < min:
          min = b  
           
  for y in range(0, 16):
    for x in range(0, 16):
      px = matrix[y][x]   
      py = y           
      if px != -1 and py != -1:
        xx = px + xmargin
        yy = py + ymargin
        r, g, b = rgb.getpixel((xx,yy))
#        r = 20
#        g = 10
#        b = 0
#        print str(r)

#        r = r / 4
#        g = g / 4
#        b = b / 4

#        r = r - min
#        g = g - min
#        b = b - min

        r = r / 4
        g = g / 4
        b = b / 4

        cmd = str(r) + "," + str(g) + "," + str(b) + ":rgb"
#        print cmd
        command(cmd)      
  command("flu")

def setup(): 
  global s, num_leds, play_time 
  s = serial.Serial("/dev/ttyS0", 115200) 
  flush_input()
  command(":::")
  command("pau:pau")
  command("-1:sch")

#  if len(sys.argv) > 1:
#    command(sys.argv[1])
#
#  if len(sys.argv) > 2:
#    num_leds = int(sys.argv[2])
#
#  if len(sys.argv) > 3:                                                                                                               
#    size_range_max = int(sys.argv[3])    
#
#  if len(sys.argv) > 4:
#    size_range_min = int(sys.argv[4])
#
#  if len(sys.argv) > 5:
#    play_time = float(sys.argv[5])

def loop():
  grab_image()
  rgb = convert_image()
  display_image(rgb)


if __name__ == '__main__': 
  setup() 
  #while True: 
  loop()


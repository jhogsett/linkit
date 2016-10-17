#!/usr/bin/python

import serial 
import time
s = None

def flush_input():
  s.read(s.inWaiting())

def wait_for_ack():
  while s.inWaiting() == 0:
    pass
  flush_input()

def command(cmd_text):
  s.write((cmd_text + ':').encode())   
  wait_for_ack()

def setup(): 
  global s 
  s = serial.Serial("/dev/ttyS0", 115200) 
  command("pause:64:window:erase")

effects = ['blink1','blink2','blink3','blink4','blink5','blink6']

def loop(): 
  for i in range(0, 64):
    command("1," + str((63-i) + 1) + ":pshifto")
    effect = effects[5 - (i % 6)]
    if(i < 63):
      command(str(63-i) + ":window:random:" + effect)
    else:
      command("random:" + effect)
  command("64:window:continue")
  time.sleep(15)
  command("pause")

if __name__ == '__main__': 
  setup() 
  while True: 
    loop()

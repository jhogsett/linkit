#!/usr/bin/python

import serial 
import time
import sys

s = None
inter_command_delay = 0.1

def flush_input():                        
  s.flushInput()
                                        
def wait_for_ack():                       
  while s.inWaiting() <= 0:               
    pass                                  
  s.read(s.inWaiting())                   

def command(cmd_text):
  s.write((':::' + cmd_text + ':').encode())   
  wait_for_ack()

def setup(): 
  global s 
  s = serial.Serial("/dev/ttyS0", 115200) 

  if len(sys.argv) > 1:
    for arg in sys.argv:
      command(arg)
    exit()    

cmd = ""
last_cmd = cmd

def loop(): 
  global cmd,last_cmd
  try:
    cmd = raw_input('command: ')
    if cmd == "":
      command(last_cmd)
    else:
     command(cmd)
     last_cmd = cmd

  except KeyboardInterrupt:                                                                         
    sys.exit("\nExiting...\n")                                                    
  except Exception:                           
    raise  

if __name__ == '__main__': 
  setup() 
  while True: 
    loop()


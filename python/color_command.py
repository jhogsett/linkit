#!/usr/bin/python

import serial 
import time
import sys

response_wait = 0.1
s = None                        

def flush_input():              
  s.flushInput()  

def flush_output():
  s.flushOutput()

def wait_for_ack():
  time.sleep(response_wait)
  while s.inWaiting() <= 0:
    pass
  time.sleep(response_wait)
  while s.inWaiting() > 0:
    print s.read(s.inWaiting()),
  print

def command(cmd_text):
  s.write((cmd_text + ':\0:').encode())   
  #print (cmd_text + ':').encode()
  wait_for_ack()

def setup(): 
  global s 
  s = serial.Serial("/dev/ttyS0", 115200) 

  if len(sys.argv) > 1:
    for arg in sys.argv[1:]:
      command(arg)
      flush_output();
    sys.exit("\nSent.\n")    

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
    flush_output();

  except EOFError:
    sys.exit("\nExiting...\n")
  except KeyboardInterrupt:                                                                         
    sys.exit("\nExiting...\n")                                                    
  except Exception:                           
    raise  

if __name__ == '__main__': 
  setup() 
  while True: 
    loop()


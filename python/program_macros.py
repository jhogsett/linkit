#!/usr/bin/python

import serial 
import time
import sys

def flush_input():                        
  s.flushInput()
                                        
def wait_for_ack():                       
  while s.inWaiting() <= 0:               
    pass
  while s.inWaiting() > 0:
    print s.read(s.inWaiting()),
  print                   

def command(cmd_text):
#  s.write((':::' + cmd_text + ':').encode())   
  s.write((cmd_text + ':').encode())
  wait_for_ack()

def set_macro(macro, macro_text):
  print "macro " + str(macro) + ": ",
  command(str(macro) + ":set:" + macro_text)
#  command("1," + str(macro) + ":tst")

s = None                                                       
def setup(): 
  global s 
  s = serial.Serial("/dev/ttyS0", 115200) 

  if len(sys.argv) > 1:
    for arg in sys.argv:
      command(arg)
    sys.exit("\nSent.\n")    

#cmd = ""
#last_cmd = cmd

def loop(): 
  # tunsten lamp                                 
  set_macro(10, "clr:-1:sch:1:pau:21:run");     
  set_macro(10, "clr:-1:sch:1:pau:21:run");
                                                 
  # sparking colors with drain                   
  set_macro(11, "clr:100,13:sch:10000,14:sch"); 
  
  # swinging random colors                                               
  set_macro(12, "clr:30:run");                  
                                                 
  set_macro(13, "-1:rps:1:rnd:sfd:flu:rst");    
  set_macro(14, "15,15:run:500:del");           
  set_macro(15, "16:run:17:run:18:run:flu"); 
  set_macro(16, "1:zon:1:blk:2:zon:1:rev:1:blk:rst");
  set_macro(17, "3:zon:1:blk:4:zon:1:rev:1:blk:rst");
  set_macro(18, "5:zon:1:blk:6:zon:1:rev:1:blk:rst");
                                                      
  set_macro(21, "era:50:lev:13,120:run:tun:flo:cfa:100:lev");
                                                              
  set_macro(30, "1:pau:40:run:44:run:46:run");              
                                                      
  set_macro(31, "0:sqs:pos:0:pal:sfd:rst:flu");             
  set_macro(32, "1:sqs:pos:1:pal:sfd:rst");                 
  set_macro(33, "2:sqs:pos:2:pal:sfd:rst");                 
  set_macro(34, "3:sqs:pos:3:pal:sfd:rst");                 
  set_macro(35, "4:sqs:pos:4:pal:sfd:rst");         
  set_macro(36, "5,:sqs:pos:5:pal:sfd:rst");                
                                                              
  set_macro(37, "0,15,0:sqs:1,30,15:sqs");                  
  set_macro(38, "2,45,30:sqs:3,60,45:sqs");         
  set_macro(39, "4,75,60:sqs:5,90,75:sqs");                 
  set_macro(40, "37:run:38:run:39:run");            
                                                              
  set_macro(41, "21,31:sch:22,32:sch");             
  set_macro(42, "23,33:sch:24,34:sch");                     
  set_macro(43, "25,35:sch:26,36:sch");                     
  set_macro(44, "41:run:42:run:43:run");                    
                                                              
  set_macro(45, "3:shf");                                   
  set_macro(46, "700,45:sch");   

if __name__ == '__main__': 
  setup() 
  loop()


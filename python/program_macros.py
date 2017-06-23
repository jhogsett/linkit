#!/usr/bin/python

import serial 
import time
import sys

response_wait = 0.1
s = None                                                     
debug_mode = False  

def flush_input():                        
  s.flushInput()
                                        
def wait_for_ack():                       
  while s.inWaiting() <= 0:               
    pass
  time.sleep(response_wait);
  while s.inWaiting() > 0:
    print s.read(s.inWaiting()),
  print                   

def wait_for_int():
  while s.inWaiting() <= 0:
    pass
  time.sleep(response_wait);
  intstr = ""
  while s.inWaiting() > 0:
    intstr = intstr + s.read(s.inWaiting())
  try:
    return int(intstr[:-1])
  except ValueError:
    print "whoops " + intstr
    return 0

def wait_for_str():
  while s.inWaiting() <= 0:
    pass
  time.sleep(response_wait);
  str = ""
  while s.inWaiting() > 0:
    str = str + s.read(s.inWaiting())
  return str[:-1]

def command(cmd_text):
  s.write((cmd_text + ':').encode())
  wait_for_ack()

def command_int(cmd_text):
  s.write((cmd_text + ':').encode())
  return wait_for_int()

def command_str(cmd_text):       
  s.write((cmd_text + ':').encode()) 
  return wait_for_str()                     

def set_macro(macro, macro_text, expected_bytes):
  if debug_mode:
    print "macro " + str(macro) + ": ",
  bytes = command_int(str(macro) + ":set:" + macro_text)
  if debug_mode:                                             
    print str(bytes) + " bytes"                                    
    print command_str("1," + str(macro) + ":tst")
  else:
    sys.stdout.write('.')
    sys.stdout.flush()
  if expected_bytes > 0 and expected_bytes != bytes:
    print "Oops! wrong number of bytes received - retrying"
    set_macro(macro, macro_text, expected_bytes)
    
def setup(): 
  global s, debug_mode 
  s = serial.Serial("/dev/ttyS0", 115200) 
  command(":::stp:stp")

  if len(sys.argv) > 3:                                       
    if(sys.argv[3] == "debug"):
      debug_mode = True

def apollo_macros(): 
  print "apollo macros:"

  # tunsten lamp                                 
  set_macro(10, "clr:-1:sch:1:pau:21:run", 11);
                                                 
  # sparking colors with drain                   
  set_macro(11, "clr:100,13:sch:10000,14:sch", 13); 
  
  # swinging random colors                                               
  set_macro(12, "clr:30:run", 4);                  
                                                 
  set_macro(13, "-1:rps:1:rnd:sfd:flu:rst", 10);    
  set_macro(14, "15,15:run:500:del", 10);           
  set_macro(15, "16:run:17:run:18:run:flu", 10); 
  set_macro(16, "1:zon:1:blk:2:zon:1:rev:1:blk:rst", 16);
  set_macro(17, "3:zon:1:blk:4:zon:1:rev:1:blk:rst", 16);
  set_macro(18, "5:zon:1:blk:6:zon:1:rev:1:blk:rst", 16);
                                                      
  set_macro(21, "era:50:lev:13,120:run:tun:flo:cfa:100:lev", 19);
                                                              
  set_macro(30, "1:pau:40:run:44:run:46:run", 12);              

                                                           
  set_macro(31, "0:ssc:pos:0:pal:sfd:rst:flu", 10);      
  set_macro(32, "1:ssc:pos:1:pal:sfd:rst", 9);           
  set_macro(33, "2:ssc:pos:2:pal:sfd:rst", 9);           
  set_macro(34, "3:ssc:pos:3:pal:sfd:rst", 9);           
  set_macro(35, "4:ssc:pos:4:pal:sfd:rst", 9);                   
  set_macro(36, "5:ssc:pos:5:pal:sfd:rst", 9);           
                                                                 
  set_macro(37, "0,16,0:ssc:1,31,15:ssc", 14);           
  set_macro(38, "2,44,30:ssc:3,60,44:ssc", 16);                  
  set_macro(39, "4,75,59:ssc:5,90,74:ssc", 16);          
  set_macro(40, "37:run:38:run:39:run", 9);    
                                                      
#  set_macro(31, "0:ssp:pos:0:pal:sfd:rst:flu", 10);             
#  set_macro(32, "1:ssp:pos:1:pal:sfd:rst", 9);                 
#  set_macro(33, "2:ssp:pos:2:pal:sfd:rst", 9);                 
#  set_macro(34, "3:ssp:pos:3:pal:sfd:rst", 9);                 
#  set_macro(35, "4:ssp:pos:4:pal:sfd:rst", 9);         
#  set_macro(36, "5:ssp:pos:5:pal:sfd:rst", 9);                
                                                              
#  set_macro(37, "0,16,0:ssp:1,31,15:ssp", 14);                  
#  set_macro(38, "2,44,30:ssp:3,60,44:ssp", 16);         
#  set_macro(39, "4,75,59:ssp:5,90,74:ssp", 16);                 
#  set_macro(40, "37:run:38:run:39:run", 9);            
                                                              
  set_macro(41, "21,31:sch:22,32:sch", 12);             
  set_macro(42, "23,33:sch:24,34:sch", 12);                     
  set_macro(43, "25,35:sch:26,36:sch", 12);                     
  set_macro(44, "41:run:42:run:43:run", 9);                    
                                                              
  set_macro(45, "shf", 1);                                   
  set_macro(46, "700,45:sch", 6);   

def default_macros():
  apollo_macros()

def threeway_macros():
  print "three way macros"

  set_macro(10, "19:run");

  set_macro(11, "wht:brt:brt:flo:flu:30:del");
  set_macro(12, "rnd:brt:brt:sfd:flo:flu");
  set_macro(13, "11:run:12:run");
  set_macro(14, "13:run:20000,10000,14:rng:sch");
    
  set_macro(15, "rps:wht:brt:brt:sfd:rst");
  set_macro(16, "rps:1:rnd:sfd:flu:rst");
  set_macro(17, "fad:50,15:sch:20000,14:sch");
  set_macro(18, "fad:50,16:sch");
    
  set_macro(19, "-1:sch:60000,19,20:sch:17:run");
  set_macro(20, "-1:sch:60000,19,21:sch:18:run");
  set_macro(21, "-1:sch:60000,19,19:sch:23:run");

  set_macro(22, "-3:rng:zon:art:flu:rst");
  set_macro(23, "fad:2:rnd:flo:flu:50,22:sch");

def oneway_macros():
  print "one way macros"

  set_macro(10, "era:50,11:sch:15,12:sch:200,16:sch");
  set_macro(11, "rng:pos:rnd:twi:flu:rst");
  set_macro(12, "rng:pos:sfd:flu:rst");
  set_macro(13, "-3:rng:zon");
  set_macro(14, "2:rng:rev");
  set_macro(15, "0,3:rng:art");
  set_macro(16, "13:run:14:run:15:run:rst");

def loop():                                  
  if len(sys.argv) > 1:
    if sys.argv[1] == "apollo":
      apollo_macros()
    elif sys.argv[1] == "threeway":
      threeway_macros()
    elif sys.argv[1] == "oneway":
      oneway_macros()
  else:
    default_macros()

  if len(sys.argv) > 2:                                      
    command(sys.argv[2])

  print "done"

if __name__ == '__main__': 
  setup() 
  loop()


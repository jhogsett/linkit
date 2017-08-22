#!/usr/bin/python

import serial 
import time
import sys
import terminal_colors as tc

response_wait = 0.1
s = None                                                     
debug_mode = False  
macro_count = 0

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

def write(text):                                        
  sys.stdout.write(text)                                
  sys.stdout.flush() 

def set_macro(macro, macro_text, expected_bytes):
  global macro_count
  if debug_mode:
    print "macro " + str(macro) + ": ",
  bytes = command_int(str(macro) + ":set:" + macro_text)
  command_str("grn:flu")                                 
  macro_count += 1
  if debug_mode:                                             
    print str(bytes) + " bytes"                                    
    print command_str("1," + str(macro) + ":tst")
  else:
    write(tc.green('.'))
    if expected_bytes > 0 and expected_bytes != bytes:
      print "Wrong number of bytes received for macro #" + str(macro) + "- retrying"
      set_macro(macro, macro_text, expected_bytes)
    
def setup(): 
  global s, debug_mode 
  s = serial.Serial("/dev/ttyS0", 115200) 
  command_str(":::stp:stp")

  if len(sys.argv) > 3:                                       
    if(sys.argv[3] == "debug"):
      debug_mode = True

######################################################################

def monument_macros():
  print tc.cyan("monument macros:\n")
 
  set_macro(10, "20:lev:16:run:11:run:12:run", 0)
  set_macro(11, "3:zon:0,3:pal:3,0,4:cpy", 0)
  set_macro(12, "2:zon:0,3:pal:3,0,2:cpy", 0)
  set_macro(13, "3:shf:11:run:12:run:rst", 0)
  set_macro(14, "3:zon:art:flu:rst", 0)
  set_macro(15, "2:zon:art:flu:rst", 0)
  set_macro(16, "800,14:sch:1600,15:sch:60000,13:sch", 0)

# process_commands(F("10:set:100:lev:800,14:sch:1600,15:sch:60000,13:sch"));
# process_commands(F("11:set:3:zon:0,2:pal:2,0,6:cpy"));
# process_commands(F("12:set:2:zon:0,2:pal:2,0,3:cpy"));
# process_commands(F("13:set:3:shf:11:run:12:run:rst"));
# process_commands(F("14:set:3:zon:art:flu:rst"));
# process_commands(F("15:set:2:zon:art:flu:rst"));


def annunciator_macros():
  print tc.cyan("annunciator macros:\n")

  # m
  set_macro(10, "-1:sch:22:run", 0)
  set_macro(11, "-1:sch:23:run", 0)

  # sequence the next window for each display
  set_macro(13, "0:snw:pos:0:pal:sfd:flo:rst:flu", 0)
  set_macro(14, "1:snw:pos:1:pal:sfd:flo:rst", 0)
  set_macro(15, "2:snw:pos:2:pal:sfd:flo:rst", 0)

  # set up each sequencer
  #set_macro(16, "0,13,0:ssc:1,25,12:ssc", 0)
  #set_macro(17, "2,37,24:ssc", 0)

  set_macro(16, "0,13,0:sqs:1,25,12:sqs", 0)
  set_macro(17, "2,37,24:sqs", 0)

  # set up all sequences and a quick fade rate
  set_macro(18, "16:run:17:run:3,8000:cfg", 0)

  # set up each each display macro schedule
  set_macro(19, "145,13:sch:150,14:sch", 0)
  set_macro(20, "155,15:sch", 0)

  # set up all display macro schedules
  set_macro(21, "19:run:20:run", 0)

  # start up random color change power swingers
  set_macro(22, "1:pau:18:run:21:run:31:run", 0)

  # start up hue sequence swingers
  set_macro(23, "1:pau:18:run:21:run:32:run", 0)

  # set up shuffler
  set_macro(30, "shf", 1)

  # set shuffler schedule
  set_macro(31, "13000,30:sch", 0)

  # set up sequencing hues
  set_macro(32, "3,360:seq:500,33:sch", 0)
  set_macro(33, "3,0,1:seq:sto:120:rcl:csh", 0)






def apollo_macros(): 
  print tc.cyan("apollo macros:\n")

  # tunsten lamp                                 
  set_macro(12, "clr:-1:sch:1:pau:21:run", 11)
                                                 
  # sparking colors with drain                   
  set_macro(11, "clr:100,13:sch:10000,14:sch", 13) 
  
  # swinging random colors                                               
  set_macro(10, "clr:30:run:25:run", 0)                  
                                                 
  set_macro(13, "-1:rps:1:rnd:sfd:flu:rst", 10)    
  set_macro(14, "15,15:run:500:del", 10)           
  set_macro(15, "16:run:17:run:18:run:flu", 10) 
  set_macro(16, "1:zon:1:blk:2:zon:1:rev:1:blk:rst", 16)
  set_macro(17, "3:zon:1:blk:4:zon:1:rev:1:blk:rst", 16)
  set_macro(18, "5:zon:1:blk:6:zon:1:rev:1:blk:rst", 16)
                                                      
  set_macro(21, "era:50:lev:13,120:run:tun:flo:cfa:100:lev", 16)
                                                              
  set_macro(30, "1:pau:40:run:44:run:46:run:1:shf", 15)              
                                                           
  set_macro(31, "0:snw:pos:0:pal:sfd:flo:rst:flu", 11)      
  set_macro(32, "1:snw:pos:1:pal:sfd:flo:rst", 10)           
  set_macro(33, "2:snw:pos:2:pal:sfd:flo:rst", 10)          
  set_macro(34, "3:snw:pos:3:pal:sfd:flo:rst", 10)           
  set_macro(35, "4:snw:pos:4:pal:sfd:flo:rst", 10)                   
  set_macro(36, "5:snw:pos:5:pal:sfd:flo:rst", 10)           
                                                                 
  set_macro(37, "0,16,0:ssc:1,31,15:ssc", 14)           
  set_macro(38, "2,46,30:ssc:3,61,45:ssc", 16)                  
  set_macro(39, "4,76,60:ssc:5,91,75:ssc", 16)          
  set_macro(40, "37:run:38:run:39:run", 9)    
                                                      
  set_macro(41, "21,31:sch:22,32:sch", 12)             
  set_macro(42, "23,33:sch:24,34:sch", 12)                     
  set_macro(43, "25,35:sch:26,36:sch", 12)                     
  set_macro(44, "41:run:42:run:43:run", 9)                    
                                                              
  # set_macro(45, "shf", 1)                                   
  # set_macro(46, "700,45:sch", 6)   
  set_macro(46, "0,45:sch", 0)

  set_macro(25, "6,360:seq:100,26:sch", 0)
  set_macro(26, "6,0,2:seq:sto:60:rcl:csh", 0)

def default_macros():
  apollo_macros()

def threeway_macros():
  print tc.cyan("three way macros\n")

  set_macro(10, "19:run", 0);

  set_macro(11, "wht:brt:brt:flo:flu:30:del", 0);
  set_macro(12, "rnd:brt:brt:sfd:flo:flu", 0);
  set_macro(13, "11:run:12:run", 0);
  set_macro(14, "13:run:20000,10000,14:rng:sch", 0);
    
  set_macro(15, "rps:wht:brt:brt:sfd:rst", 0);
  set_macro(16, "rps:1:rnd:sfd:flu:rst", 0);
  set_macro(17, "fad:50,15:sch:20000,14:sch", 0);
  set_macro(18, "fad:50,16:sch", 0);
    
  set_macro(19, "-1:sch:60000,19,20:sch:17:run", 0);
  set_macro(20, "-1:sch:60000,19,21:sch:18:run", 0);
  set_macro(21, "-1:sch:60000,19,19:sch:23:run", 0);

  set_macro(22, "-3:rng:zon:art:flu:rst", 0);
  set_macro(23, "fad:2:rnd:flo:flu:50,22:sch", 0);

def oneway_macros():
  print tc.cyan("one way macros\n")

  set_macro(10, "era:50,11:sch:15,12:sch:200,16:sch", 0);
  set_macro(11, "rng:pos:rnd:twi:flu:rst", 0);
  set_macro(12, "rng:pos:sfd:flu:rst", 0);
  set_macro(13, "-3:rng:zon", 0);
  set_macro(14, "2:rng:rev", 0);
  set_macro(15, "0,3:rng:art", 0);
  set_macro(16, "13:run:14:run:15:run:rst", 0);

def loop():                                  
  print
  if len(sys.argv) > 1:
    if sys.argv[1] == "apollo":
      apollo_macros()
    elif sys.argv[1] == "threeway":
      threeway_macros()
    elif sys.argv[1] == "oneway":
      oneway_macros()
    elif sys.argv[1] == "annunciator":
      annunciator_macros()
    elif sys.argv[1] == "monument":
      monument_macros()
  else:
    default_macros()

  command_str("cnt:10:run")

  if len(sys.argv) > 2:                                      
    command_str(sys.argv[2])

  print "\n" 
  print tc.green(str(macro_count) + " macros successfully programmed\n")

if __name__ == '__main__': 
  print tc.magenta("\nApollo Lighting System - Macro Programmer v0.0\n")                         

  setup() 
  loop()


#!/usr/bin/python

import serial 
import time
import sys

response_wait = 0.1
s = None                                                     
debug_mode = False  
num_tests = 0
num_failures = 0

def flush_input():                        
  s.flushInput()
                                        
def wait_for_ack():                       
  while s.inWaiting() <= 0:               
    pass
  time.sleep(response_wait);
  while s.inWaiting() > 0:
    s.read(s.inWaiting()),

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

def set_macro(macro, macro_text):
  print "macro " + str(macro) + ": ",
  bytes = command_int(str(macro) + ":set:" + macro_text)
  print str(bytes) + " bytes"
  if debug_mode:                                             
    print command_str("1," + str(macro) + ":tst")

def setup(): 
  global s, debug_mode 
  s = serial.Serial("/dev/ttyS0", 115200) 
  command(":::stp:stp")

#  if len(sys.argv) > 3:                                       
#    if(sys.argv[3] == "debug"):
#      debug_mode = True

test_number = None
test_description = None

def normal():                                              
  return "\x1b[39m" 

def red(text):
  print "\x1b[31m" + text + normal(),
  sys.stdout.flush()

def green(text):                                              
  print "\x1b[32m" + text + normal(),
  sys.stdout.flush()                                              

def cyan(text):
  print "\x1b[36m" + text + normal(),
  sys.stdout.flush()                                              

def blue(text):                                                   
  print "\x1b[34m" + text + normal(),                             
  sys.stdout.flush()   

num_leds = 0

def test(number, description):
  global test_number, test_description, num_leds
  print
  test_number = number
  test_description = description 
  num_leds = command_int("0,0:tst")              
  cyan("test #" + str(test_number) + " " + test_description + " ")
  command(":::stp:stp")

def fail(got, expected):
  red("failed! expected: " + expected + " got: " + got)
  print

def succeed():
  green(".")

def expect_equal(got, expected):
  if got != expected:
    fail(got, expected)
  else:
    succeed()

def spec_1():
  test(1, "it sets a pre-rendered red value in the buffer")
  command("red")
  str = command_str("2,0,1:tst")
  expect_equal(str, "20,0,0,")

def spec_2():
  test(2, "it doesn't render while paused")
  command("red")
  str = command_str("3,0,1:tst")                           
  expect_equal(str, "0,0,0,")                             

def spec_3():
  test(3, "it renders a rendered red value in the render buffer")
  command("red:flu")
  str = command_str("3,0,1:tst")
  expect_equal(str, "38,0,0,")              

def spec_4():
  test(4, "it erases the rendered value")
  command("era:flu")
  str = command_str("3,0,1:tst")                                 
  expect_equal(str, "0,0,0,")  

def spec_5():
  test(5, "it repeats the color value only once")
  command("grn:rep:flu")
  str = command_str("2,0,1:tst") 
  expect_equal(str, "0,20,0,")
  str = command_str("2,1,1:tst")           
  expect_equal(str, "0,20,0,")  
  str = command_str("2,2,1:tst")           
  expect_equal(str, "0,0,0,") 

def spec_6():
  test(6, "it floods all leds")
  command("pur:flo:flu")
  num_leds = command_int("0,0:tst")
  for i in range(num_leds):
    str_ = command_str("2," + str(i) + ",1:tst")                                  
    expect_equal(str_, "10,0,20,")                                    
   
def spec_7():
  test(7, "it mirrors the pattern accurately")
  command("cyn:yel:mag:mir:flu")
  str_ = command_str("2,0,3:tst")
  expect_equal(str_, "20,0,20,20,20,0,0,20,20,")
  str_ = command_str("2," + str(num_leds - 3) + ",3:tst")                 
  expect_equal(str_, "0,20,20,20,20,0,20,0,20,")
  
def loop():                                  
  spec_1()
  spec_2()
  spec_3()
  spec_4()
  spec_5()
  spec_6()
  spec_7()






#  if len(sys.argv) > 2:                                      
#    command(sys.argv[2])


if __name__ == '__main__': 
  setup() 
  loop()


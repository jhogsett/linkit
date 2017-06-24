#!/usr/bin/python

# ideas: flash green or red depending on tests when done
#        arg to run a single test

import serial 
import time
import sys

response_wait = 0.1
s = None                                                     
debug_mode = False  
num_tests = 0
num_failures = 0
test_number = 0                                                                                                   
test_description = None                                                                                           
test_failures = []                                                                                                
test_command = None                                                                                               
success_count = 0                                                                                                 
failure_count = 0                                                                                                 
group_number = 0                                                                                                  
group_description = None  

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
  global test_command
  test_command = cmd_text
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

colors = [
  ("red", "20,0,0"),
  ("org", "20,10,0"),
  ("yel", "20,20,0"),
  ("grn", "0,20,0"),
  ("blu", "0,0,20"),                                             
  ("pur", "10,0,20"),                                             
  ("cyn", "0,20,20"),                                             
  ("mag", "20,0,20"),                                             
  ("lbl", "0,10,20"),                                             
  ("lgr", "10,20,0"),                                             
  ("sea", "0,20,10"),                                             
  ("pnk", "20,0,10"),                                             
  ("amb", "20,15,0"),                                             
  ("olv", "15,20,0"),                                             
  ("sky", "0,15,20"),                                             
  ("tur", "0,20,15"),                                             
  ("lav", "15,0,20"),                                             
  ("ros", "20,0,15"),                                             
  ("blk", "0,0,0"),                                             
  ("dgr", "5,5,5"),                                             
  ("gry", "10,10,10"),                                             
  ("wht", "20,20,20"),                                             
  ("tun", "20,11,2")                                             
]

def normal():                                              
  return "\x1b[39m" 

def write(text):
  sys.stdout.write(text)
  sys.stdout.flush()                                                

def red(text):
  return "\x1b[31m" + text + normal()

def green(text):                                              
  return "\x1b[32m" + text + normal()

def cyan(text):
  return "\x1b[36m" + text + normal()

def blue(text):                                                   
  return "\x1b[34m" + text + normal()                             

num_leds = 0

def group(description):                                                                    
  global group_number, group_description
  group_number = group_number + 1
  group_description = description

def test(description):
  global test_number, test_description, test_failures, num_leds
  test_number = test_number + 1
  test_description = description 
  num_leds = command_int("0,0:tst")              
  command(":::stp:stp:20:lev")

def fail(got, expected):
  global test_failures, failure_count
  test_failures.append(blue("Group #" + str(group_number) + " " + group_description))
  test_failures.append(cyan("  Test #" + str(test_number) + " " + test_description)) 
  test_failures.append(red("    Command '" + test_command + "' failed! expected: " + expected + " got: " + got + "\n"))
  write(red("F"))
  failure_count += 1

def succeed():
  global success_count
  write(green("."))
  success_count += 1

def expect_equal(got, expected):
  if got != expected:
    fail(got, expected)
  else:
    succeed()

def expect_buffer(command_, start, count, expected):
  command(command_)
  str_ = command_str("2," + str(start) + "," + str(count) + ":tst")                                 
  expect_equal(str_[:-1], expected)

def expect_render(command_, start, count, expected):               
  command(command_)                                                
  str_ = command_str("3," + str(start) + "," + str(count) + ":tst")
  expect_equal(str_[:-1], expected)                                
                                                                   
def expect_effect(command_, start, count, expected):               
  command(command_)                                                
  str_ = command_str("4," + str(start) + "," + str(count) + ":tst")
  expect_equal(str_[:-1], expected)                                
                                                                   
def expect_palette(command_, start, count, expected):               
  #command(command_)                                                
  #str_ = command_str("5," + str(start) + "," + str(count) + ":tst")
  #expect_equal(str_[:-1], expected)                                
  pass                                                                

########################################################################
########################################################################

def specs():
  group("pushing colors to display buffer")

  test("it sets a pre-rendered value in the buffer")
  expect_buffer("red", 0, 1, "20,0,0")

  test("it sets an alternate cyan value in the buffer")
  expect_buffer("cyn", 0, 1, "0,20,20") 

  for color in colors:
    expect_buffer(color[0] + ":flu", 0, 1, color[1])

  group("pause and continue")

  test("it doesn't render while paused")
  expect_render("red", 0, 1, "0,0,0")

  group("rendering colors to the render buffer")

  test("it renders a rendered red value in the render buffer")
  expect_render("red:flu", 0, 1, "51,0,0")

  group("erasure")

  test("it erases the rendered value")
  expect_render("red:flu", 0, 1, "51,0,0")
  expect_render("era:flu", 0, 1, "0,0,0")

  group("repeating")

  test("it repeats the color value only once")
  expect_buffer("grn:rep:flu", 0, 3, "0,20,0,0,20,0,0,0,0")

  group("flooding")

  test("it floods all leds")
  num_leds = command_int("0,0:tst")
  expected_buffer = ("10,0,20," * num_leds)[:-1]
  expect_buffer("pur:flo:flu", 0, num_leds, expected_buffer)

  group("mirroring")
   
  test("it mirrors the pattern accurately")
  expect_buffer("cyn:yel:mag:mir:flu", 0, 3, "20,0,20,20,20,0,0,20,20")
  expect_buffer("", num_leds - 3, 3, "0,20,20,20,20,0,20,0,20")

  group("pushing multiple colors")

  test("it places two colors (only)")
  expect_buffer("2:yel:flu", 0, 3, "20,20,0,20,20,0,0,0,0")

  group("pushing effects to the effects buffer")

  test("it places an effect in the effects buffer")
  expect_effect("org:bli:flu", 0, 1, "10")

  group("positioning")

  test("pos sets the next insertion postion and default 0 width")
  expect_buffer("1:pos:red:flu", 0, 3, "0,0,0,20,0,0,0,0,0")

  test("pos sets the offset + width")
  expect_buffer("1,2:pos:wht:flo:flu", 0, 4, "0,0,0,20,20,20,20,20,20,0,0,0")
  # offset into zone

  group("copying")
  group("palette manipulation")                                                            
  group("zones")                                                                          
  group("setting offset and window")                                                                          
  group("reverse and forward")                                                                          
  group("rgb color")                                                                          
  group("hsl color")                                                                          
  group("custom black level")                                                                          
  group("")                                                                          
  group("")                                                                          
  group("")                                                                          

########################################################################                     
########################################################################                     
 
def loop():                                  
#  num_specs = 11;
#  print "Running " + str(num_specs) + " specs"
#  for spec_number in range(1, num_specs + 1):
#    getattr(sys.modules[__name__], "spec_%s" % str(spec_number))()

  specs()
  print "\n" 

  for error in test_failures:
    print error

  print str(success_count + failure_count) + " expectations, " + green(str(success_count) + " succeeded ") + red(str(failure_count) + " failed")

#  if len(sys.argv) > 2:                                      
#    command(sys.argv[2])

if __name__ == '__main__': 
  setup() 
  loop()
  print


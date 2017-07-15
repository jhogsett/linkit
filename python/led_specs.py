#!/usr/bin/python

# TODO: standard seed should be set for each test, not for each group 

# ideas:
#        arg to run a single test

import serial 
import time
import sys
import inspect
import terminal_colors as tc
import test_colors

def get_line_number(back):
  callerframerecord = inspect.stack()[back]    # 0 represents this line, 1 represents line at caller                                                                                                                       
  frame = callerframerecord[0]                                                                                                                                                                  
  info = inspect.getframeinfo(frame)                                                                                                                                                            
  return info.lineno  

response_wait = 0.2
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
test_line_num = 0
num_pending = 0
last_group_number = 0                                                  
last_test_number = 0 
num_skipped = 0
default_brightness = None                                                                                                                                                              
default_brightness_percent = None                                                                                                                                                  
color_divisor = 20.0                                                                                                                                                                                     
standard_seed = 1
num_groups = 0
test_failure_summaries = []
num_leds = 0                                                                                                                                                                                               
palette_size = 0
group_number_only = -1
standard_palette = ""

# --- Serial I/O ---

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

# --- Sending Commands ---

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

# --- Setup ---

def setup(): 
  global s, debug_mode, num_leds, default_brightness, default_brightness_percent, palette_size, group_number_only, standard_palette 
  s = serial.Serial("/dev/ttyS0", 115200) 
  do_reset_device()
  num_leds = command_int("0,0:tst")                                                                                                                                                        
  palette_size = command_int("0,1:tst")
  default_brightness = command_int("0,4:tst")                                                                                                
  default_brightness_percent = default_brightness / 100.0                                                                                                               
  for i in range(0, palette_size):
    standard_palette += test_colors.colors[i][1] + ","
  standard_palette = standard_palette[:-1]

  if len(sys.argv) > 2:
    group_number_only = int(sys.argv[2])

  print (
          tc.cyan("Device: ") + 
          tc.white(str(num_leds) + 
          " LEDs, default brightness: " + 
          str(default_brightness) + "%")
	)                                                                                                                                                  

def write(text):
  sys.stdout.write(text)
  sys.stdout.flush()                                                


# --- device handling ---

def reset_device():
  return ":::stp:stp:20:lev"

def reset_standard_seed():
  return "6,3," + str(standard_seed) + ":tst"

def reset_standard_fade_rate():
  return "3,9995:cfg"

def reset_standard_palette():
  return "1:shf"

def pre_test_reset():
  command = ""
  command += reset_device() + ":"
  command += reset_standard_seed() + ":"
  command += reset_standard_fade_rate() + ":"
  command += reset_standard_palette()
  command_str(command)

def do_reset_device():
  command_str(reset_device())                                                                                                                                                                             


# --- test definition ---

def group(description):                                                                    
  global group_number, group_description, last_group_number, num_groups
  group_number = group_number + 1
  num_groups += 1
  group_description = description

def test(description):
  global test_number, test_description, test_failures, last_test_number
  test_number = test_number + 1
  test_description = description 
  pre_test_reset()

def pending_test(description):                                                                                                                                                                             
  global test_number, test_description, test_line_number, num_pending                                                                                                                                      
  test_line_number = get_line_number(2)                                                                                                                                                                    
  test_number = test_number + 1                                                                                                                                                                            
  test_description = description                                                                                                                                                                           
  report_pending()                                                                                                                                                                                         
  num_pending += 1                                                                                                                                                                                         
  write(tc.yellow("."))                                                                                                                                                                       
                                                                                                                                                                                                           
def skip_test(command, description):                                                                                                                                                                       
  global test_number, test_description, test_line_number, num_skipped                                                                                                                                      
  test_line_number = get_line_number(2)                                                                                                                                                                    
  test_number = test_number + 1                                                                                                                                                                            
  test_description = description                                                                                                                                                                           
  report_skipped()                                                                                                                                                                                         
  num_skipped += 1                                                                                                                                                                                         
  write(tc.red("."))                                                                                                                                                                          


# --- reporting results ---
                                                                                                                                                                                                           
def report_group():
  global last_group_number
  if group_number != last_group_number:                                                                                                                                                                    
    test_failures.append(tc.blue("\nGroup #" + str(group_number) + " " + group_description))                                                                                                                    
    last_group_number = group_number                                                       

def report_test():
  global last_test_number
  report_group()
  if test_number != last_test_number:                                                                                                                                                                      
    test_failures.append(tc.yellow("  Test #" + str(test_number)) + " " + tc.cyan(test_description))                                                                                                                     
    last_test_number = test_number                                                      

def report_failure(got, expected):
  report_test()
  test_failures.append(
    "    " + 
    tc.red("Expectation: ") + 
    tc.cyan("[" + test_command + "]") + 
    tc.yellow(" @ " + str(test_line_number)) + 
    tc.red(" Failed!\n") + 
    tc.yellow("\texpected:\n") + 
    tc.red("\t\t[" + expected + "]\n") + 
    tc.yellow("\tgot:\n") + 
    tc.green("\t\t[" + got + "]") + 
    "\n")

  test_failure_summaries.append(
    tc.yellow("\t@ " + str(test_line_number) + " ") + 
    tc.cyan(test_command) + 
    tc.red(" -" + expected) + 
    tc.green(" +" + got) + 
    "\n") 

def report_pending():
  report_test()
  test_failures.append(
    "    " + 
    tc.yellow("Pending expectation: ") + 
    tc.cyan("[" + test_description + "]") + 
    tc.yellow(" @ " + str(test_line_number))) 

def report_skipped():                                                                                                                                                                                      
  report_test()                                                                                                                                                                                            
  test_failures.append(
    "    " + 
    tc.red("Skipped expectation: ") + 
    tc.red("[" + test_command + "] ") + 
    tc.red("[" + test_description + "]") + 
    tc.yellow(" @ " + str(test_line_number)))                                                             


# --- failing/succeeding tests ---
                                                                                      
def fail(got, expected):
  global test_failures, failure_count, last_group_number, last_test_number
  report_failure(got, expected)
  write(tc.red("F"))
  failure_count += 1
  last_group_number = group_number
  last_test_number = test_number

def succeed():
  global success_count
  write(tc.green("."))
  success_count += 1


# --- expectations ---

def expect_equal(got, expected):
  global test_line_number
  test_line_number = get_line_number(3)
  if got != expected:
    fail(got, expected)
  else:
    succeed()

def expect_not_equal(got, expected):
  global test_line_number
  test_line_number = get_line_number(3)
  if got == expected:
    fail(got, "not:" + expected)
  else:
    succeed()

def expect_macro(command_, macro, expected):
  command(command_)
  str_ = command_str("1," + str(macro) + ":tst")
  count = len(expected)
  expect_equal(str_[:count], expected)

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
                                                                   
def expect_palette(command_, start, count, expected, positive=True):               
  display_width = num_leds / palette_size                                                                                                                         
  display_command = ":" + str(palette_size) + ",-2," + str(display_width) + ":cpy:flu"  
  command(command_ + display_command)                                                
  str_ = command_str("5," + str(start) + "," + str(count) + ":tst")
  if positive:
    expect_equal(str_[:-1], expected)                                
  else:
    expect_not_equal(str_[:-1], expected)

def expect_int(command_, expected):
  got = command_int(command_)
  expect_equal(str(expected), str(got))                                                                  

def expect_empty_buffer(command_, start, count):
  str_ = ""
  for i in range(count):
    str_ += "0,0,0,"
  expect_buffer(command_, start, count, str_[:-1])

def expect_empty_render(command_, start, count):
  str_ = ""
  for i in range(count):
    str_ += "0,0,0,"
  expect_render(command_, start, count, str_[:-1])


# --- helper functions ---

def rgb_string(red, green, blue):
  return str(red) + "," + str(green) + "," + str(blue)

def rgb_strings(red, green, blue):
  return str(red) + "," + str(green) + "," + str(blue) + ","

def unscaled_color_value(rgb_color_value):
  return int(((rgb_color_value / default_brightness_percent) / 255) * color_divisor)

def rendered_color_value(buffer_color_value):
  return int(((buffer_color_value / color_divisor) * 255) * default_brightness_percent) 



########################################################################
########################################################################

def specs():
########################################################################
# PUSHING COLORS
########################################################################
  group("pushing colors to display buffer")

  test("it sets a pre-rendered red value in the buffer")
  expect_buffer("red", 0, 1, "20,0,0")

  test("it sets an alternate cyan value in the buffer")
  expect_buffer("cyn", 0, 1, "0,20,20") 

  test("it accurately sets all standard colors")
  for color in test_colors.colors:
    expect_buffer(color[0] + ":flu", 0, 1, color[1])


########################################################################
# PUSHING MULTIPLE COLORS
########################################################################
  group("pushing multiple colors")                                                                                     
                                                                                                                       
  test("it places two colors (only)")                                                                                  
  expect_buffer("2:yel:flu", 0, 3, "20,20,0,20,20,0,0,0,0")                          

  test("it places multiple colors in reverse mode")
  expect_buffer("1:rev:2:sea:flu", num_leds - 3, 3, "0,0,0,0,20,10,0,20,10")
                                                                           

########################################################################
# PAUSE AND CONTINUE
########################################################################  
  group("pause and continue")

  test("it doesn't render while paused")
  expect_render("red", 0, 1, "0,0,0")

  # test pausing and resuming schedules and effects


########################################################################
# RENDER BUFFER
########################################################################
  group("rendering colors to the render buffer")

  test("it renders a rendered blue value in the render buffer")
  expect_render("blu:flu", 0, 1, "0,0,51")

  test("it renders an alternate orange value in the render buffer")
  expect_render("org:flu", 0, 1, "51,25,0")


########################################################################
# ERASURE
########################################################################
  group("erasure")

  test("it erases the rendered value")
  expect_render("red:flu", 0, 1, "51,0,0")
  expect_render("era:flu", 0, 1, "0,0,0")

  test("it erases only within the set window")
  expect_render("6:pnk:flu", 0, 6, "51,0,25,51,0,25,51,0,25,51,0,25,51,0,25,51,0,25")
  expect_render("2:off:4:win:era:flu", 0, 6, "51,0,25,51,0,25,0,0,0,0,0,0,51,0,25,51,0,25")

  test("it erases within the set window in reverse mode")
  expect_render("1:rev:6:pnk:flu", num_leds - 6, 6, "51,0,25,51,0,25,51,0,25,51,0,25,51,0,25,51,0,25")                                                                                                                                                              
  # offset and window are always in reference to pixel 0 regardless of reversal
  expect_render(str(num_leds - 4) + ":off:" + str(num_leds - 2) + ":win:era:flu", num_leds - 6, 6, "51,0,25,51,0,25,0,0,0,0,0,0,51,0,25,51,0,25") 

  # test erasing to custom black level


########################################################################
# REPEATING
########################################################################
  group("repeating")

  test("it repeats the color value only once")
  expect_buffer("grn:rep:flu", 0, 3, "0,20,0,0,20,0,0,0,0")

  test("it repeats the color value multiple times")
  expect_buffer("grn:2:rep:flu", 0, 4, "0,20,0,0,20,0,0,20,0,0,0,0")

  # repeating works in reverse mode
  test("it repeats properly in reverse mode")
  expect_buffer("1:rev:gry:rep:flu", num_leds - 3, 3, "0,0,0,10,10,10,10,10,10")

  test("it repeats properly in reverse modei multiple times")
  expect_buffer("1:rev:gry:2:rep:flu", num_leds - 4, 4, "0,0,0,10,10,10,10,10,10,10,10,10")


########################################################################
# FLOODING
########################################################################
  group("flooding")

  test("it floods all leds")
  expected_buffer = ("10,0,20," * num_leds)[:-1]
  expect_buffer("pur:flo:flu", 0, num_leds, expected_buffer)

  test("it floods only within the set window")
  expect_buffer("2:off:4:win:ros:flo:flu", 0, 6, "0,0,0,0,0,0,20,0,15,20,0,15,0,0,0,0,0,0")

  # not sure how to test this
  # pending_test("it does no flooding if there's no room")

  test("it floods properly in reverse mode")
  expected_buffer = ("20,15,0," * num_leds)[:-1]                                                                                                                                                           
  expect_buffer("1:rev:amb:flo:flu", 0, num_leds, expected_buffer)  


########################################################################
# MIRRORING
########################################################################
  group("mirroring")
   
  test("it mirrors the pattern accurately")
  expect_buffer("cyn:yel:mag:mir:flu", 0, 3, "20,0,20,20,20,0,0,20,20")
  expect_buffer("", num_leds - 3, 3, "0,20,20,20,20,0,20,0,20")

  test("it mirrors only within the set window")
  expect_buffer("10:win:grn:blu:mir:flu", 0, 10, "0,0,20,0,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,20,0,0,0,20")

  test("it mirrors only within the set offset and window")
  expect_buffer("10:off:20:win:mag:lgr:mir:flu", 10, 10, "10,20,0,20,0,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,20,0,20,10,20,0")

  test("it mirrors properly in reverse mode") 
  expect_buffer("1:rev:lbl:pnk:mir:flu", num_leds - 10, 10, "0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,10,20,20,0,10")
  expect_buffer("", 0, 10, "20,0,10,0,10,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0")

  test("it mirrors properly in reverse mode within an offset and window")
  expect_buffer("1:rev:10:off:20:win:red:pur:mir:flu", 10, 10, "10,0,20,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,20,0,0,10,0,20")


########################################################################
# EFFECTS BUFFER
########################################################################
  group("pushing effects to the effects buffer")

  test("it places an effect in the effects buffer")
  expect_effect("org:bli:flu", 0, 1, "10")

  pending_test("it places an alternate effect in the effects buffer")

  pending_test("it places multiple effects in the effects buffer")
 

########################################################################
# POSITIONING
########################################################################
  group("positioning")

  test("pos sets the next insertion postion and default 0 width")
  expect_buffer("1:pos:red:flu", 0, 3, "0,0,0,20,0,0,0,0,0")

  test("pos sets the offset + width")
  expect_buffer("1,2:pos:wht:flo:flu", 0, 4, "0,0,0,20,20,20,20,20,20,0,0,0")

  test("offset override is always relative to LED #0")
  expect_buffer("2:off:2:off:lav:flu", 0, 5, "0,0,0,0,0,0,15,0,20,0,0,0,0,0,0")

  test("positioning works in reverse mode")
  # positioning is always relative to LED #0 and should be unaffected by direction
  expect_buffer("0:rev:3:pos:tun:flo:flu", 0, 5, "0,0,0,0,0,0,0,0,0,20,11,2,0,0,0")                                                          
  expect_buffer("1:rev:3:pos:tun:flo:flu", 0, 5, "0,0,0,0,0,0,0,0,0,20,11,2,0,0,0")

  skip_test("1:rev:2,2:pos:lgr:flo:flu", "positioning with width works in reverse mode")
  # expect_buffer("1:rev:2,2:pos:lgr:flo:flu", 0, 4, "")                                                                                                                                  


########################################################################
# COPYING
########################################################################
  group("copying")

  pending_test("it copies the right number of pixels")
  pending_test("it copies the right number of times")
  pending_test("it fills the current width if times is zero")
  pending_test("it copies to the palette buffer if 18 or fewer pixels are being copied")
  pending_test("it copies to the render buffer if more than 18 pixels are being copied")
  pending_test("it overwrites the original position when duplicating")
  pending_test("it copies the right number of zoomed pixels")
  pending_test("it copies a pattern to the palette")
  pending_test("it pastes a pattern to the buffer")
  pending_test("it pastes the pattern at the current offset")
 

########################################################################
# PALETTE SHUFFING
########################################################################
  group("palette shuffling")                                                            

  test("the palette can be shuffled")
  expected_colors = "x20,10,0,15,20,0,0,0,20,0,20,0,0,10,20,20,20,0,20,0,15,20,0,0,10,20,0,20,0,10,0,20,10,10,0,20,20,15,0,20,0,20,0,15,20,0,20,15,15,0,20,0,20,20"
  expect_palette("shf", 0, palette_size, expected_colors)

  test("the palette resets to the right fixed set of colors")
  expect_palette("shf:1:shf", 0, palette_size, standard_palette)

  test("the shuffler sets every odd-numbered palette color to the previous one's compliment")
  expected_colors = "x20,0,0,0,20,20,20,20,0,0,0,20,0,0,20,20,20,0,0,20,20,20,0,0,0,10,20,20,10,0,0,20,10,20,0,10,20,15,0,0,5,20,0,15,20,20,5,0,15,0,20,5,20,0"
  expect_palette("shf:2:shf", 0, palette_size, expected_colors)                                                                         

  test("the shuffler creates complimentary color pairs for the standard palette")
  expected_colors = "x20,10,0,0,10,20,0,0,20,20,20,0,0,10,20,20,10,0,20,0,15,0,20,5,10,20,0,10,0,20,0,20,10,20,0,10,20,15,0,0,5,20,0,15,20,20,5,0,15,0,20,5,20,0"
  expect_palette("3:shf", 0, palette_size, expected_colors)                                                                 

  test("the shuffler compliments the entire current palette")
  expected_colors = "x20,10,0,0,10,20,0,0,20,20,20,0,0,10,20,20,10,0,20,0,15,0,20,5,10,20,0,10,0,20,0,20,10,20,0,10,20,15,0,0,5,20,0,15,20,20,5,0,15,0,20,5,20,0"   
  expect_palette("shf:4:shf", 0, palette_size, expected_colors)                                                                                             

  test("the shuffler rotates the current palettes down")
  expected_colors = "x20,10,0,0,10,20,0,0,20,20,20,0,0,10,20,20,10,0,20,0,15,0,20,5,10,20,0,10,0,20,0,20,10,20,0,10,20,15,0,0,5,20,0,15,20,20,5,0,15,0,20,5,20,0"
  expect_palette("shf:5:shf", 0, palette_size, expected_colors)

  test("the shuffler rotates the current palette up")
  expected_colors = "x20,10,0,0,10,20,0,0,20,20,20,0,0,10,20,20,10,0,20,0,15,0,20,5,10,20,0,10,0,20,0,20,10,20,0,10,20,15,0,0,5,20,0,15,20,20,5,0,15,0,20,5,20,0"
  expect_palette("shf:6:shf", 0, palette_size, expected_colors)

  test("the shuffler reverses the current palette")
  expected_colors = "x20,10,0,0,10,20,0,0,20,20,20,0,0,10,20,20,10,0,20,0,15,0,20,5,10,20,0,10,0,20,0,20,10,20,0,10,20,15,0,0,5,20,0,15,20,20,5,0,15,0,20,5,20,0"
  expect_palette("shf:7:shf", 0, palette_size, expected_colors)


########################################################################
# ZONES
########################################################################
  group("zones")                                                                          

  pending_test("choosing a zone sets the offset and window")
  pending_test("choosing the main zone resets to 0, num_leds")
                                                                                                                                                                                                            

########################################################################
# OFFSET AND WINDOW
########################################################################
  group("setting offset and window")                                                                          
                                                              
  test("an offset can be set")
  expect_buffer("1:off:grn:flu", 0, 2, "0,0,0,0,20,0")

  test("a window can be set")
  expect_buffer("2:win:neo:flo:flu", 0, 2, "20,5,0,20,5,0")

  test("pushed-off-the-end colors don't exceed the window boundary")
  expect_buffer("2:win:lbl:flo:lbl:flu", 0, 2, "0,10,20,0,10,20")

  test("setting an offset is not relative to the current offset")
  expect_buffer("1:off:1:off:lgr:flu", 0, 2, "0,0,0,10,20,0")

  skip_test("2:win:4:off:dgr", "the window is adjusted if the offset is set beyond the current window")
  #command_str("2:win:4:off:dgr:flu")
  #current_offset = command_str("0,2:tst")
  #current_window = command_str("0,3:tst")
  #expect_int("0,2:tst", 4)
  #expect_int("0,3:tst", 4)
                                                                                                                                           

########################################################################
# REVERSE AND FORWARD
########################################################################
  group("reverse and forward")                                                                          
          
  pending_test("the insertion mode can be set to reverse")
  pending_test("the insertion mode can be set to normal")


########################################################################
# PUSHING RGB COLORS
########################################################################
  group("rgb color")                                                                          

  color_value = 255
  unscaled_color_value_ = unscaled_color_value(color_value)
  rendered_color_value_ = rendered_color_value(unscaled_color_value_)

  # compute pre-rendered value for full-brightness pixel
  expected_rgb_color = rgb_string(unscaled_color_value_, unscaled_color_value_, unscaled_color_value_)

  test("it unscales to the proper pre-rendered value")
  expect_buffer("255,255,255:rgb:flu", 0, 1, expected_rgb_color)

  # compute rendered value for recovered full-brightness pixel
  expected_render_value = rendered_color_value_
  expected_rgb_color = rgb_string(expected_render_value, expected_render_value, expected_render_value)

  test("it renders the expected RGB value in the render buffer")                                                                                                        
  # must render at default brightness to recover the original value
  expect_render(str(default_brightness) + ":lev:255,255,255:rgb:flu", 0, 1, expected_rgb_color)                                                         

  test("current brightness level doesn't affect unscaling calculation")
  expect_render("1:lev:255,255,255:rgb:" + str(default_brightness) + ":lev:flu", 0, 1, expected_rgb_color)                         
                                                                  

########################################################################
# PUSHING HSL COLORS
########################################################################
  group("hsl color")                                                                          

  color_value = 255                                                                         
  unscaled_color_value_ = unscaled_color_value(color_value)                                                                                                                                                
  rendered_color_value_ = rendered_color_value(unscaled_color_value_)                                                                                                                                      
                                                         
  test("it sets the expected HSL value in the display buffer")                                                                                                                                     
  expected_rgb_color = str(unscaled_color_value_) + ",0,0"
  expect_buffer("0,255,255:hsl:flu", 0, 1, expected_rgb_color)

  test("it renders the expected HSL value in the render buffer")                                                                                                                                   
  expected_rgb_color = str(rendered_color_value_) + ",0,0"                                                                                                                                                 
  expect_render(str(default_brightness) + ":lev:0,255,255:hsl:flu", 0, 1, expected_rgb_color)                                                                                                          
                
  test("current brightness level doesn't affect unscaling calculation")                                                                                                                                    
  expect_render("1:lev:0,255,255:hsl:" + str(default_brightness) + ":lev:flu", 0, 1, expected_rgb_color)
                                                                                                                                    

########################################################################
# CUSTOM BLACK LEVEL
########################################################################
  group("custom black level")                                                                          
  
  pending_test("a custom black level can be set")
  pending_test("erases uses the custom black level")
  pending_test("placing 'black' uses the custom black level")
                                                                                                                                                                                                         

########################################################################
# PUSHING RANDOM COLORS
########################################################################
  group("random color")                                                                          
                                                             
  test("it chooses a random color")
  expect_buffer("rnd:flu", 0, 1, "15,20,0")  

  test("it chooses another random color")
  expect_buffer("rnd:flu", 0, 1, "15,20,0")                                                                                                                                 

  test("it sets no effect")
  expect_effect("rnd:flu", 0, 1, "0")

  test("it repeats using the same color not another random color")               
  expect_buffer("rnd:rep:flu", 0, 2, "15,20,0,15,20,0")  

  test("it floods using the same color not another random color")
  expect_buffer("rnd:flo:flu", 0, 2, "15,20,0,15,20,0")

  test("it sets a random color and sets the effect to repeat with random colors")
  expect_buffer("1:rnd:flu", 0, 1, "15,20,0")                                                                                        
  expect_effect("1:rnd:flu", 0, 1, "1")  

  test("it repeats using a different color")
  expect_buffer("1:rnd:rep:flu", 0, 2, "20,0,20,15,20,0")

  test("it floods using a different color each time")                                                                                                                                                      
  if num_leds == 90:  
    expect_buffer("1:rnd:flo:flu", 0, 3, "15,20,0,20,0,20,10,0,20") 
  elif num_leds == 100:
    expect_buffer("1:rnd:flo:flu", 0, 3, "15,20,0,0,20,20,0,10,20")
  else:
    expect_buffer("1:rnd:flo:flu", 0, 3, "15,20,0,20,0,20,10,0,20")

  test("the repeated colors get no effect set")
  expect_effect("1:rnd:rep:rep:flu", 0, 3, "0,0,1") 

  test("the flooded colors get no effect set")                          
  expect_effect("1:rnd:flo:flu", 0, 3, "1,0,0")    

  test("it sets a random color and sets the effect to repeat with random colors+effects")                                                                                                                          
  expect_buffer("2:rnd:flu", 0, 1, "15,20,0")                                                                                                                                                                    
  expect_effect("2:rnd:flu", 0, 1, "2") 

  test("it repeats using a different color")                                                                                                                                                               
  expect_buffer("2:rnd:rep:flu", 0, 2, "10,0,20,15,20,0")                                                                                                              
                                                                                                                                                                                                           
  test("it floods using a different color each time")                                                 
  if num_leds == 90:
    expect_buffer("2:rnd:flo:flu", 0, 3, "15,20,0,20,0,20,20,20,0")                                                                                                                                          
  elif num_leds == 100:
    expect_buffer("2:rnd:flo:flu", 0, 3, "15,20,0,0,20,20,20,20,0")
  else:
    expect_buffer("2:rnd:flo:flu", 0, 3, "15,20,0,20,0,20,20,20,0")

  test("the flooded colors get random effects set")                                     
  expect_effect("2:rnd:flo:flu", 0, 3, "2,0,16") 

  skip_test("2:rnd:rep:rep:flu", "the repeated colors get random effects set")                                                                                                                                                            
  #expect_effect("2:rnd:rep:rep:flu", 0, 3, "x0,0,2")                                                                                                                                                        


########################################################################
# BLENDING COLORS
########################################################################
  group("blending colors")                                                                          

  pending_test("it blends two colors @ 50%")
  pending_test("it blends two colors @ 90%")
  pending_test("it blends two colors @ 10%")
                                                                                                                                                                                                           

########################################################################
# MAX, DIM AND BRIGHT
########################################################################
  group("max, dim and bright")                                                                          

  pending_test("it boosts the brightness level")
  pending_test("it reduces the brightness level")
  pending_test("it maxxes out the brightness level")
                                                                                                                                                                                                           

########################################################################
# BLINK EFFECTS
########################################################################
  group("blink effects")                                                             

  pending_test("it renders at the minimum brightness level when blink is off")
  pending_test("it renders at the current brightness level when blink is on")

  pending_test("the standard blink alternates correctly")
  pending_test("the a/b blinks alternate correctly")
  pending_test("the 6-blink chase chases correctly")
  pending_test("the custom blink alternates correctly")
        
  # alternate custom rates
                                                                                                                                                                                                   

########################################################################
# BREATHE EFFECT
########################################################################
  group("breathe effect")                                                             
                 
  pending_test("the right breathe brightness levels are used for rendering")
                                                                                                                                                                                          

########################################################################
# FADING EFFECTS
########################################################################
  group("fade effects")                                                             
        
  test("it modifies the display color with slow fades on flushing")
  expect_buffer("red:sfd:flu", 0, 1, "19,0,0")

  expect_buffer("flu", 0, 1, "18,0,0")
  expect_buffer("flu", 0, 1, "17,0,0")

  test("it renders the fading color with slow fades on flushing")
  expect_render("red:sfd:flu", 0, 1, "48,0,0")
  expect_render("flu", 0, 1, "45,0,0")
  expect_render("flu", 0, 1, "43,0,0")

  test("a custom fade rate modifies the display buffer properly")
  command_str("3,7500:cfg")
  expect_buffer("red:sfd:flu", 0, 1, "15,0,0")
  expect_buffer("flu", 0, 1, "11,0,0")
  expect_buffer("flu", 0, 1, "8,0,0")

  test("a custom fade rate renders properly")
  command_str("3,7500:cfg")
  expect_render("red:sfd:flu", 0, 1, "38,0,0")
  expect_render("flu", 0, 1, "28,0,0")
  expect_render("flu", 0, 1, "20,0,0")

  pending_test("it fast fades properly")


########################################################################
# RESET CLEAR AND STOP
########################################################################
  group("reset, clear and stop")                                                             
  
                                                                                                                                                                                                         
########################################################################
# BRIGHTNESS LEVEL
########################################################################
  group("brightness level")                                                             
                                                                                                                                                                                                           

########################################################################
# FADE ANIMATION
########################################################################
  group("fade animation")                                                             

  test("it leaves the buffer empty (black) after done")
  expect_empty_buffer("amb:flo:flu:fad", 0, num_leds)                                                                                                                                                                                                           

  test("it leaves the display empty (black) after done")
  expect_empty_render("olv:flo:flu:fad", 0, num_leds)


########################################################################
# WIPE ANIMATION
########################################################################
  group("wipe animation")                                                             
                                                                                                                                                                                                           

########################################################################
# ANIMATED ROTATION
########################################################################
  group("animated rotation")


########################################################################
# ROTATION
########################################################################
  group("rotation")                                                             

  test("it rotates within the current window")
  expect_buffer("0:off:5:win:red:rot:flu", 0, 5, "0,0,0,20,0,0,0,0,0,0,0,0,0,0,0")

  test("it rotates in reverse in the current window")
  expect_buffer("0:off:5:win:blu:1:rev:rot:flu", 0, 5, "0,0,0,0,0,0,0,0,0,0,0,0,0,0,20")

  test("it rotates multiple times within the current window")
  expect_buffer("0:off:5:win:red:2:rot", 0, 5, "0,0,0,0,0,0,20,0,0,0,0,0,0,0,0")

  test("it rotates multiple times in reverse in the current window")
  expect_buffer("0:off:5:win:blu:1:rev:2:rot", 0, 5, "0,0,0,0,0,0,0,0,0,0,0,20,0,0,0")                                                                                                                                                                                                           


########################################################################
# POWER SHIFT
########################################################################
  group("power shift")                                                             
                                                                                                                                                                                                           

########################################################################
# CROSSFADE ANIMATION
########################################################################
  group("crossfade")                                                             
                                                                                                                                                                                                           

########################################################################
# SETTING CUSTOM BLINK PERIOD
########################################################################
  group("setting custom blink period")                                                             
                                                                                                                                                                                                           

########################################################################
# SETTING BLINK PERIOD
########################################################################
  group("setting blink period")                                                             
                                                                                                                                                                                                           

########################################################################
# CARRY COLOR
########################################################################
  group("carry color")                                                                                                            
                                                                                                                                                                                                           

########################################################################
# CUSTOM BREATHE TIME
########################################################################
  group("setting custom breathe time")                                                                                                            
                                                                                                                                                                                                           

########################################################################
# MACROS
########################################################################
  group("setting and running macros")                                                                                                            

  test("a known bug is fixed - using values 50-59 as arguments in setting macros uses too many bytes")
  for x in range(49,61):
    expect_macro("rnd:flu:0:set:" + str(x), 0, "249," + str(x) + ",255")                                                                                                                                                                                                           


########################################################################
# DELAY
########################################################################
  group("delay")                                                                                                            
                                                                                                                                                                                                           

########################################################################
# RANDOM NUMBER
########################################################################
  group("random number")                                                                                                            
                                                                                                                                                                                                           

########################################################################
# POSITION
########################################################################
  group("position")                                                                                                            
                                                                                                                                                                                                           

########################################################################
# RANDOM POSITION
########################################################################
  group("random postion")                                                                                                            
                                                                                                                                                                                                           

########################################################################
# SEQUENCING
########################################################################
  group("sequencing")                                                                                                            
                                                                                                                                                                                                           

########################################################################
# TESTING
########################################################################
  group("testing")                                                                                                            


########################################################################
# PALETTE COLOR SWEEPING
########################################################################
  group("palette color sweeping")     

  test("it sweeps the right default hues")                                                                                  
  expected_colors = "20,0,0,20,6,0,20,13,0,20,20,0,13,20,0,6,20,0,0,20,0,0,20,6,0,20,13,0,20,20,0,13,20,0,6,20,0,0,20,6,0,20,13,0,20,20,0,20,20,0,13,20,0,6"    
  expect_palette("csh", 0, palette_size, expected_colors)                                                                                                 

  test("it sweeps hues with a custom step angle")                                                                                                                  
  expected_colors = "20,0,0,20,13,0,13,20,0,0,20,0,0,20,13,0,13,20,0,0,20,13,0,20,20,0,13,20,0,0,20,13,0,13,20,0,0,20,0,0,20,13,0,13,20,0,0,20,13,0,20,20,0,13"       
  expect_palette("40:csh", 0, palette_size, expected_colors) 
        
  test("it sweeps hues with a custom starting angle")                                                                                                                  
  expected_colors = "20,20,0,13,20,0,6,20,0,0,20,0,0,20,6,0,20,13,0,20,20,0,13,20,0,6,20,0,0,20,6,0,20,13,0,20,20,0,20,20,0,13,20,0,6,20,0,0,20,6,0,20,13,0"                                                                                                                                             
  expect_palette("0,60:csh", 0, palette_size, expected_colors)                                                                                                       
                                                                                                                                                            
  test("it sweeps hues with a custom lightness")                                                                                                                  
  expected_colors = "10,0,0,10,3,0,10,6,0,10,10,0,6,10,0,3,10,0,0,10,0,0,10,3,0,10,6,0,10,10,0,6,10,0,3,10,0,0,10,3,0,10,6,0,10,10,0,10,10,0,6,10,0,3"                                                                                                                                             
  expect_palette("0,0,10:csh", 0, palette_size, expected_colors)                                                                                                       
                                                                                                                                                            

  test("it sweeps the right default saturations")                                                                                                                        
  expected_colors = "20,0,0,20,1,1,20,2,2,20,3,3,20,4,4,20,5,5,20,6,6,20,7,7,20,8,8,20,9,9,20,11,11,20,12,12,20,13,13,20,14,14,20,15,15,20,16,16,20,17,17,20,18,18"                                                                                                                                                    
  expect_palette("css", 0, palette_size, expected_colors)                                                                                                         
                                                                                                                                                              
  test("it sweeps saturations with a custom hue")                                                                                                                  
  expected_colors = "0,20,0,1,20,1,2,20,2,3,20,3,4,20,4,5,20,5,6,20,6,7,20,7,8,20,8,9,20,9,11,20,11,12,20,12,13,20,13,14,20,14,15,20,15,16,20,16,17,20,17,18,20,18"
  expect_palette("120:css", 0, palette_size, expected_colors)                                                                                                          
                                                                                                                                          
  test("it sweeps saturations with a custom step")                                                                                                                  
  expected_colors = "20,0,0,20,2,2,20,4,4,20,6,6,20,8,8,20,10,10,20,13,13,20,15,15,20,17,17,20,19,19,20,1,1,20,4,4,20,6,6,20,8,8,20,10,10,20,12,12,20,15,15,20,17,17"
  expect_palette("0,28:css", 0, palette_size, expected_colors)                                                                                                          
                                                                                                                                          
  test("it sweeps saturations with a custom lightness")                                                                                                                  
  expected_colors = "10,0,0,10,0,0,10,1,1,10,1,1,10,2,2,10,2,2,10,3,3,10,3,3,10,4,4,10,4,4,10,5,5,10,6,6,10,6,6,10,7,7,10,7,7,10,8,8,10,8,8,10,9,9"
  expect_palette("0,0,10:css", 0, palette_size, expected_colors)                                                                                                          
                                                                                                                                          
  
  test("it sweeps the right default lightnesses")                                                                                                                        
  expected_colors = "1,0,0,2,0,0,3,0,0,4,0,0,5,0,0,6,0,0,7,0,0,8,0,0,9,0,0,11,0,0,12,0,0,13,0,0,14,0,0,15,0,0,16,0,0,17,0,0,18,0,0,19,0,0"                                                                                                                                                    
  expect_palette("csl", 0, palette_size, expected_colors)                                                                                                         
                                                                                                                                                              
  test("it sweeps lightnesses with a custom hue")                                                                                                                  
  expected_colors = "0,0,1,0,0,2,0,0,3,0,0,4,0,0,5,0,0,6,0,0,7,0,0,8,0,0,9,0,0,11,0,0,12,0,0,13,0,0,14,0,0,15,0,0,16,0,0,17,0,0,18,0,0,19"                         
  expect_palette("240:csl", 0, palette_size, expected_colors)                                                                                                          

  test("it sweeps lightnesses with a custom step")                                                                                         
  expected_colors = "2,0,0,4,0,0,6,0,0,8,0,0,10,0,0,13,0,0,15,0,0,17,0,0,19,0,0,1,0,0,4,0,0,6,0,0,8,0,0,10,0,0,12,0,0,15,0,0,17,0,0,19,0,0"                                                                                                                                             
  expect_palette("0,28:csl", 0, palette_size, expected_colors)                                                                                                          
                                                                                                                                                                   
  test("it sweeps lightnesses with a custom lightness scale factor")                                                                                         
  expected_colors = "0,0,0,1,0,0,1,0,0,2,0,0,2,0,0,3,0,0,3,0,0,4,0,0,4,0,0,5,0,0,6,0,0,6,0,0,7,0,0,7,0,0,8,0,0,8,0,0,9,0,0,9,0,0"                                                                                                                                             
  expect_palette("0,0,10:csl", 0, palette_size, expected_colors)                                                                                                          
                                                                                                                                                                   
  # test that all three args are 0 after the test runs, saw arg2 being 1 as green success leds were pushed out 
                                                                                                                                                                                                           
########################################################################                     
########################################################################                     
 
def loop():                                  
  print
  specs()
  test("")
  print 

  for error in test_failures:
    print error

  if(failure_count > 0):
    print tc.red("\nFailures:")
    for summary in test_failure_summaries:                                                                                                                                                                              
      print summary,

  print
  print (
    tc.cyan(str(success_count + failure_count) + " expectations ") + 
    tc.green(str(success_count) + " succeeded ") + 
    tc.red(str(failure_count) + " failed - ") + 
    tc.yellow(str(num_pending) + " pending ") + 
    tc.red(str(num_skipped) + " skipped ") + 
    tc.blue(str(num_groups) + " groups")
  )                                                                     
  print                                                                                                                                                                                            

  total = success_count + failure_count + num_pending + num_skipped
  show_success = 0.5 + (success_count * num_leds / total)
  show_failure = 0.5 + ((failure_count + num_skipped) * num_leds / total)
  show_pending = 0.5 + (num_pending * num_leds / total)
  command_str("rst:era")
  command_str(str(show_success) + ",1:grn") 
  command_str(str(show_failure) + ",1:red")                                                                                                                                                                  
  command_str(str(show_pending) + ",1:yel")                                                                                                                                                                  
  command_str("flu:cnt")

if __name__ == '__main__': 
  print tc.magenta("\nApollo Lighting System - Test Framework v0.0\n")
  setup() 
  loop()
  print

#def print_frame():
#  callerframerecord = inspect.stack()[1]    # 0 represents this line
#                                            # 1 represents line at caller
#  frame = callerframerecord[0]
#  info = inspect.getframeinfo(frame)
#  print info.filename                       # __FILE__     -> Test.py
#  print info.function                       # __FUNCTION__ -> Main
#  print info.lineno                         # __LINE__     -> 13


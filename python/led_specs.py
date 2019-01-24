#!/usr/bin/python

# in verbose, don't show dots, instead show text of test in color

import serial 
import time
import sys
import inspect
import terminal_colors as tc
import test_colors
import argparse
import led_command as lc
import app_ui as ui

app_description = "Apollo Lighting System - Test Framework v0.0 - Aug 10, 2017"
slow_response_wait = 0.15
fast_response_wait = 0.01
response_wait = fast_response_wait
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
test_line_number = 0
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
group_number_only = 0
standard_palette = ""
alternate_seed = 2
group_line_number = 0
verbose_mode = False
verbose_test_outcome = ""
group_name_only = ""
test_number_only = 0
skip_led_report = None
quiet_mode = None


def initialize():
  global s, debug_mode, num_leds, default_brightness, default_brightness_percent, palette_size, group_number_only, standard_palette, verbose_mode, group_name_only, test_number_only, skip_led_report, quiet_mode

  parser = argparse.ArgumentParser(description=app_description)
  parser.add_argument("-g", "--group",     type=int, dest="group",      default=0, help="group number to test")
  parser.add_argument("-n", "--groupname",           dest="groupname",  default="", help="group name matching text to test")
  parser.add_argument("-t", "--test",      type=int, dest="test",       default=0, help="test number to test")
  parser.add_argument("-s", "--skip-report",         dest="skip_report",action='store_true', help="skip showing results on LED display")
  parser.add_argument("-v", "--verbose",             dest="verbose",    action='store_true', help="enable verbose mode")
  parser.add_argument("-q", "--quiet", dest="quiet", action="store_true", help="don't use terminal colors (False)")

  args = parser.parse_args()
  group_number_only = args.group
  group_name_only = args.groupname
  test_number_only = args.test
  verbose_mode = args.verbose
  skip_led_report = args.skip_report
  quiet_mode = args.quiet

  tc.begin(quiet_mode)
  ui.begin(verbose_mode, quiet_mode)
  lc.begin(verbose_mode)

  do_reset_device()
  num_leds = lc.get_num_leds()
  palette_size = lc.get_palette_size()
  default_brightness = lc.get_default_brightness()

  introduction()

  default_brightness_percent = default_brightness / 100.0

  for i in range(0, palette_size):
    standard_palette += test_colors.colors[i][1] + ","
  standard_palette = standard_palette[:-1]

  if not lc.get_test_framework_enabled():
    ui.report_error("Test framework is not enabled for this device.")
    sys.exit()

def introduction():
  ui.app_description(app_description)
  ui.report_info("Device:")
  ui.info_entry("Number of LEDs", num_leds)
  ui.info_entry("Default Brightness", default_brightness)

  ui.report_verbose("verbose mode")

  if group_number_only != 0:
    ui.report_info_alt("group " + str(group_number_only) + " only")


# -----------------------------------------------------------------------------
# --- device handling ---

def reset_device():
  return ":::stp:stp:20:lev:2,0:cfg"

def reset_standard_fade_rate():
  return "2,9995:cfg"

def reset_standard_palette():
  return "1:shf"

def reset_default_effect():
  return "3,0:cfg"

def reset_standard_seed():
  return "6,3," + str(standard_seed) + ":tst"

def reset_alternate_seed():
  return "6,3," + str(alternate_seed) + ":tst"

def pre_test_reset():
  command = ""
  command += reset_device() + ":"
  command += reset_standard_seed() + ":"
  command += reset_standard_fade_rate() + ":"
  command += reset_standard_palette() + ":"
  command += reset_default_effect()
  lc.command_str(command)

def do_reset_device():
  lc.command_str(reset_device())

# -----------------------------------------------------------------------------
# --- line number reporting ---

def get_line_number(back):
  callerframerecord = inspect.stack()[back]    # 0 represents this line, 1 represents line at caller
  frame = callerframerecord[0]
  info = inspect.getframeinfo(frame)
  return info.lineno

# -----------------------------------------------------------------------------
# --- test definition ---

#TODO how to skip to another group?

def group(description):                                                                    
  global group_number, group_description, last_group_number, num_groups, group_line_number
  group_line_number = get_line_number(2)
  group_number = group_number + 1
  num_groups += 1
  group_description = description
  if test_number_only != 0 and test_number_only <= test_number:
    return False
  if group_number_only == 0 and group_name_only == "":
    if verbose_mode:
      if test_number_only == 0:
        print group_message(),
    return True
  if group_number_only == group_number:
    if verbose_mode:
      if test_number_only == 0:
        print group_message(),
    return True
  if group_name_only != "" and group_name_only.lower() in description.lower():
    if verbose_mode:
      if test_number_only == 0:
        print group_message(),
    return True
  return False

def test(description):
  global test_number, test_description, test_failures, last_test_number, test_line_number, verbose_test_outcome
  test_number = test_number + 1
  test_description = description 
  test_line_number = get_line_number(2)
  pre_test_reset()
  if test_number_only == 0:
    pre_test_reset()
    if verbose_mode:
      verbose_test_outcome = test_message()
    return True
  if test_number == test_number_only:
    pre_test_reset()
    if verbose_mode:
      print group_message(),
      print test_message(),
    return True
  return False

def pending_test(description):                                                                                                                                                                             
  global test_number, test_description, test_line_number, num_pending                                                                                                                                      
  test_line_number = get_line_number(2)                                                                                                                                                                    
  test_number = test_number + 1                                                                                                                                                                            
  test_description = description                                                                                                                                                                           
  num_pending += 1                                                                                                                                                                                         
  if test_number_only == 0 or test_number == test_number_only:
    report_pending()
    ui.report_verbose(pending_message())
    if not verbose_mode:
      ui.write(tc.yellow("*"))
                                                                                                                                                                                                           
def skip_test(command, description, num_to_skip=1):                                                                                                                                                                       
  global test_number, test_description, test_line_number, num_skipped                                                                                                                                      
  test_line_number = get_line_number(2)                                                                                                                                                                    
  test_number = test_number + num_to_skip                                                                                                                                                                           
  test_description = description                                                                                                                                                                           

  if test_number_only == 0 or test_number == test_number_only:
    report_skipped(command)
    num_skipped += num_to_skip
    ui.report_verbose(skipped_message(command))
    if not verbose_mode:
      ui.write(tc.yellow("."))

# -----------------------------------------------------------------------------
# --- reporting results ---

def group_message():
  return "\n" + tc.blue("Group #" + str(group_number) + " " + group_description) + tc.yellow(" @" + str(group_line_number))

def test_message():
  return "\n  " + tc.cyan("Test #" + str(test_number) + " " + test_description) + tc.yellow(" @" + str(test_line_number)) + " "

def failure_message(got, expected):
  return ("\n    " +
    tc.white("Expectation: ") +
    tc.cyan("[" + test_command + "]") +
    tc.yellow(" @ " + str(test_line_number)) +
    tc.red(" Failed!\n") +
    tc.white("\texpected:\n") +
    tc.red("\t\t[" + expected + "]\n") +
    tc.white("\tgot:\n") +
    tc.green("\t\t[" + got + "]") +
    "\n")

def pending_message():
  return ("\n    " +
    tc.yellow("Pending expectation: ") +
    tc.yellow("[" + test_description + "]") +
    tc.yellow(" @ " + str(test_line_number) + " "))

def skipped_message(command):
  return ("\n    " +
    tc.red("Skipped expectation: ") +
    tc.red("[" + command + "] ") +
    tc.red("[" + test_description + "]") +
    tc.yellow(" @ " + str(test_line_number)))

def report_group():
  global last_group_number
  if group_number != last_group_number:                                                                                                                                                                    
    test_failures.append(group_message()) 
    last_group_number = group_number                                                       

def report_test():
  global last_test_number
  report_group()
  if test_number != last_test_number:                                                                                                                                                                      
    test_failures.append(test_message())                                                                                                                     
    last_test_number = test_number                                                      

def report_failure(got, expected):
  report_test()
  test_failures.append(failure_message(got, expected))

  a = tc.cyan(test_command) + "\n"

  test_failure_summaries.append(
    tc.yellow("\t@ " + str(test_line_number) + " ") + 
    tc.cyan(test_command) + 
    tc.red(" -" + expected) + 
    tc.green(" +" + got) + 
    "\n") 

  if verbose_mode:
    print group_message(),    
    print test_message(),
    print failure_message(got, expected),

def report_pending():
  report_test()
  test_failures.append(pending_message())

def report_skipped(command):                                                                                                                                                                                      
  report_test()                                                                                                                                                                                            
  test_failures.append(skipped_message(command))


# -----------------------------------------------------------------------------
# --- failing/succeeding tests ---
                                                                                      
def fail(got, expected):
  global test_failures, failure_count, last_group_number, last_test_number
  report_failure(got, expected)
  failure_count += 1
  last_group_number = group_number
  last_test_number = test_number
  ui.report_verbose(verbose_test_outcome)
  if not verbose_mode:
    ui.write(tc.red("F"))

def succeed():
  global success_count
  ui.report_verbose(verbose_test_outcome)
  if not verbose_mode:
    ui.write(tc.green("."))
  success_count += 1


# -----------------------------------------------------------------------------
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
  global test_command
  test_command = command_
  lc.command(command_)
  str_ = lc.get_macro_raw(macro)
  count = len(expected)
  expect_equal(str_[:count], expected)

def expect_buffer(command_, start, count, expected, flush = True, slow = False, positive = True):
  global test_command
  test_command = command_
  if flush:
    command_ += ":flu"
  lc.command(command_)
  str_ = lc.get_buffer(start, count, slow)
  if positive:
    expect_equal(str_[:-1], expected)
  else:
    expect_not_equal(str_[:-1], expected)

def expect_render(command_, start, count, expected, flush = True, slow = False, positive = True):
  global test_command
  test_command = command_
  if flush:
    command_ += ":flu"               
  lc.command(command_)                                                
  str_ = lc.get_render(start, count, slow)
  if positive:
    expect_equal(str_[:-1], expected)                                
  else:
    expect_not_equal(str_[:-1], expected)
                                                                 
def expect_effect(command_, start, count, expected, flush = True, slow = False, positive = True):               
  global test_command
  test_command = command_
  if flush:
    command_ += ":flu"
  lc.command(command_)
  str_ = lc.get_effect(start, count, slow)
  if positive:
    expect_equal(str_[:-1], expected)                                
  else:
    expect_not_equal(str_[:-1], expected)
                                                                 
def expect_palette(command_, start, count, expected, positive=True):               
  global test_command
  test_command = command_
  display_width = num_leds / palette_size                                                                                                                         
  display_command = ":" + str(palette_size) + ",-2," + str(display_width) + ":cpy:flu"  
  lc.command(command_ + display_command)                                                
  str_ = lc.get_palette(start, count, True)
  if positive:
    expect_equal(str_[:-1], expected)                                
  else:
    expect_not_equal(str_[:-1], expected)

def expect_int(command_, expected):
  global test_command
  test_command = command_
  got = lc.command_int(command_)
  expect_equal(str(got), str(expected))                                                                  

def expect_offset(command_, expected, positive=True):
  global test_command
  test_command = command_
  lc.command_str(command_)
  got = lc.get_offset()
  if positive:
    expect_equal(str(got), str(expected))
  else:
    expect_not_equal(str(got), str(expected))

def expect_window(command_, expected, positive=True):
  global test_command
  test_command = command_
  lc.command_str(command_)
  got = lc.get_window()
  if positive:
    expect_equal(str(got), str(expected))
  else:
    expect_not_equal(str(got), str(expected))

def expect_empty_buffer(command_, start, count):
  global test_command
  test_command = command_
  expected = ""
  for i in range(count):
    expected += "0,0,0,"
  lc.command(command_)
  str_ = lc.get_buffer(start, count, True)
  expect_equal(str_[:-1], expected[:-1])

def expect_empty_render(command_, start, count):
  global test_command
  test_command = command_
  expected = ""
  for i in range(count):
    expected += "0,0,0,"
  lc.command(command_)
  str_ = lc.get_render(start, count, True)
  expect_equal(str_[:-1], expected[:-1])

def expect_accumulators(command_, expected, flush = True, positive = True):
  global test_command
  test_command = command_
  if flush:
    command_ += ":flu"
  lc.command(command_)
  str_ = lc.get_accumulator()
  if positive:
    expect_equal(str_[:-1], expected)
  else:
    expect_not_equal(str_[:-1], expected)

def get_accum0():
  str_ = lc.get_accumulator()
  return str_.split(",")[0]

def expect_sequence(setup_command, advance_command, expected_values, flush = True, positive = True):
  global test_command
  test_command = setup_command + "/" + advance_command
  got_values = []

  if flush:
    setup_command += ":flu"
    advance_command += ":flu"

  lc.command(setup_command)
  got_values.append(int(get_accum0()))

  times = len(expected_values) - 1
  for i in range(0, times):
    lc.command(advance_command)
    got_values.append(int(get_accum0()))

  if positive:
    expect_equal(str(got_values), str(expected_values))
  else:
    expect_not_equal(str(expected_values), str(expected_values))


# -----------------------------------------------------------------------------
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
  if group("pushing colors to display buffer"):

    if test("it sets a pre-rendered red value in the buffer"):
      expect_buffer("red", 0, 1, "20,0,0")

    if test("it sets an alternate cyan value in the buffer"):
      expect_buffer("cyn", 0, 1, "0,20,20") 

    if test("it accurately sets all standard colors"):
      for color in test_colors.colors:
        expect_buffer(color[0], 0, 1, color[1])

    if test("all color commands work as expected"):
      for i in range(0, len(test_colors.colors)):
        expect_buffer(test_colors.colors[i][0] + ":flu", 0, 1, test_colors.colors[i][1])

    if test("if number of times < 1 it is set to 1"):
      expect_buffer("-5:red", 0, 2, "20,0,0,0,0,0")

########################################################################
# SETTING EFFECTS
########################################################################
  if group("setting effects in the effects buffer"):

    if test("it places an effect in the effects buffer"):
      expect_effect("org:bli", 0, 1, "11")

    if test("it places an alternate effect in the effects buffer"):
      expect_effect("org:bre:flu", 0, 1, "21")

    if test("it places multiple effects in the effects buffer"):
      expect_effect("blu:bla:grn:blb", 0, 2, "19,18")

    if test("all effects are set as expected"):
      for i in range(0, len(test_colors.effects)):
        expect_effect("rnd:" + test_colors.effects[i][0] + ":flu", 0, 1, test_colors.effects[i][1])

    if test("it sets random effects"):
      expect_effect("rnd:efr", 0, 1, "21")
      expect_effect("rnd:efr", 0, 1, "15")
      expect_effect("rnd:efr", 0, 1, "12")

########################################################################
# PUSHING MULTIPLE COLORS
########################################################################
  if group("pushing multiple colors"):                                                                                     
                                                                                                                       
    if test("it places two colors (only)"):                                                                                  
      expect_buffer("2:yel", 0, 3, "20,20,0,20,20,0,0,0,0")                          

    if test("it places multiple colors in reverse mode"):
      expect_buffer("1:rev:2:sea", num_leds - 3, 3, "0,0,0,0,20,10,0,20,10")
                                                                           

########################################################################
# PAUSE AND CONTINUE
########################################################################  
  if group("pause and continue"):

    if test("it doesn't render while paused"):
      expect_render("red", 0, 1, "0,0,0", False)

    # test pausing and resuming schedules and effects
  

########################################################################
# RENDER BUFFER
########################################################################
  if group("rendering colors to the render buffer"):

    if test("it renders a rendered blue value in the render buffer"):
      expect_render("blu", 0, 1, "0,0,51")

    if test("it renders an alternate orange value in the render buffer"):
      expect_render("org", 0, 1, "51,25,0")


########################################################################
# ERASURE
########################################################################
  if group("erasure"):

    if test("it erases the rendered value"):
      expect_render("red", 0, 1, "51,0,0")
      expect_render("era", 0, 1, "0,0,0")

    if test("it erases only within the set window"):
      expect_render("6:pnk", 0, 6, "51,0,25,51,0,25,51,0,25,51,0,25,51,0,25,51,0,25")
      expect_render("2:off:4:win:era", 0, 6, "51,0,25,51,0,25,0,0,0,0,0,0,51,0,25,51,0,25")

    if test("it erases within the set window in reverse mode"):
      expect_render("1:rev:6:pnk", num_leds - 6, 6, "51,0,25,51,0,25,51,0,25,51,0,25,51,0,25,51,0,25")                                                                                                                                                              
      # offset and window are always in reference to pixel 0 regardless of reversal
      expect_render(str(num_leds - 4) + ":off:" + str(num_leds - 2) + ":win:era", num_leds - 6, 6, "51,0,25,51,0,25,0,0,0,0,0,0,51,0,25,51,0,25") 

    if test("it erases when the width is one pixel"):
      expect_buffer("3:win:olv:flo:1:pos:era", 0, 3, "15,20,0,0,0,0,15,20,0")


########################################################################
# REPEATING
########################################################################
  if group("repeating"):

    if test("it repeats the color value only once"):
      expect_buffer("grn:rep", 0, 3, "0,20,0,0,20,0,0,0,0")

    if test("it repeats the color value multiple times"):
      expect_buffer("grn:2:rep", 0, 4, "0,20,0,0,20,0,0,20,0,0,0,0")

    if test("it repeats properly in reverse mode"):
      expect_buffer("1:rev:gry:rep", num_leds - 3, 3, "0,0,0,10,10,10,10,10,10")

    if test("it repeats properly in reverse modei multiple times"):
      expect_buffer("1:rev:gry:2:rep", num_leds - 4, 4, "0,0,0,10,10,10,10,10,10,10,10,10")

    if test("it repeats the effect"):
      expect_effect("amb:bli:rep", 0, 2, "11,11")


########################################################################
# FLOODING
########################################################################
  if group("flooding"):

    if test("it floods all leds"):
      expected_buffer = ("10,0,20," * num_leds)[:-1]
      expect_buffer("pur:flo", 0, num_leds, expected_buffer, True, True)

    if test("it floods only within the set window"):
      expect_buffer("2:off:4:win:ros:flo", 0, 6, "0,0,0,0,0,0,20,0,15,20,0,15,0,0,0,0,0,0", True, True)

    # not sure how to test this
    # pending_test("it does no flooding if there's no room")

    if test("it floods properly in reverse mode"):
      expected_buffer = ("20,15,0," * num_leds)[:-1]                                                                                                                                                           
      expect_buffer("1:rev:amb:flo", 0, num_leds, expected_buffer, True, True)  


########################################################################
# MIRRORING
########################################################################
  if group("mirroring"):
   
    if test("it mirrors the pattern accurately"):
      expect_buffer("cyn:yel:mag:mir", 0, 3, "20,0,20,20,20,0,0,20,20", True, True)
      expect_buffer("", num_leds - 3, 3, "0,20,20,20,20,0,20,0,20", True, True)

    if test("it mirrors only within the set window"):
      expect_buffer("10:win:grn:blu:mir", 0, 10, "0,0,20,0,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,20,0,0,0,20")

    if test("it mirrors only within the set offset and window"):
      expect_buffer("10:off:20:win:mag:lgr:mir", 10, 10, "10,20,0,20,0,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,20,0,20,10,20,0")

    if test("it mirrors properly in reverse mode"):
      expect_buffer("1:rev:lbl:pnk:mir", num_leds - 10, 10, "0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,10,20,20,0,10")
      expect_buffer("", 0, 10, "20,0,10,0,10,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0")
 
    if test("it mirrors properly in reverse mode within an offset and window"):
      expect_buffer("1:rev:10:off:20:win:red:pur:mir", 10, 10, "10,0,20,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,20,0,0,10,0,20")


########################################################################
# POSITIONING
########################################################################
  if group("positioning"):

    if test("pos sets the next insertion postion and default 0 width"):
      expect_buffer("1:pos:red", 0, 3, "0,0,0,20,0,0,0,0,0")

    if test("pos sets the offset + width"):
      expect_buffer("1,2:pos:wht:flo", 0, 4, "0,0,0,20,20,20,20,20,20,0,0,0")

    if test("offset override is always relative to LED #0"):
      expect_buffer("2:off:2:off:lav", 0, 5, "0,0,0,0,0,0,15,0,20,0,0,0,0,0,0")

    if test("positioning in forward mode when offset+width is zero"):
      expect_buffer("0:rev:3:pos:tun:flo", 0, 5, "0,0,0,0,0,0,0,0,0,20,11,2,0,0,0")

    if test("positioning in reverse mode when offset+width is zero"):
      # in reverse mode, color is pushed one less than max and since max = offset, 
      # the position is reduced by one
      expect_buffer("1:rev:3:pos:tun:flo", 0, 5, "0,0,0,0,0,0,20,11,2,0,0,0,0,0,0")

    if test("positioning with width works in reverse mode"):
      # in reverse mode, color is pushed one less than max,
      # the start position is reduced by one
      expect_buffer("1:rev:2,2:pos:lgr:flo", 0, 5, "0,0,0,0,0,0,10,20,0,10,20,0,0,0,0")                                                                                                                                  


########################################################################
# COPYING
########################################################################
  if group("copying"):
 
    if test("it copies the pattern once"):
      expect_buffer("neo:sod:tun:flu:3,1:cpy", 0, 4, "20,11,2,20,10,4,20,5,0,0,0,0")

    if test("it copies the pattern twice"):
      expect_buffer("neo:sod:tun:flu:3,2:cpy", 0, 7, "20,11,2,20,10,4,20,5,0,20,11,2,20,10,4,20,5,0,0,0,0")

    if test("it zooms the pattern to twice as big"):
      expect_buffer("neo:sod:tun:flu:3,1,2:cpy", 0, 7, "20,11,2,20,11,2,20,10,4,20,10,4,20,5,0,20,5,0,0,0,0")

    if test("it defaults to copying to fill the default full width"):
      expected_buffer = ""
      for i in range(0, num_leds / 2):
        expected_buffer += "20,0,10,20,0,0,"
      expect_buffer("red:pnk:2:cpy", 0, num_leds, expected_buffer[:-1], True, True)
      # this currently fails for 93 leds

    if test("it fills an alternate width"):
      expected_buffer = ""
      for i in range(0, 5):
        expected_buffer += "10,10,10,0,20,20,"
      expected_buffer += "0,0,0"
      expect_buffer("10:win:cyn:gry:2:cpy", 0, 11, expected_buffer, True, True) 

    if test("it fills the complete width even if not a multiple of the pattern size"):
      expected_buffer = ""
      for i in range(0, 5):
        expected_buffer += "10,10,10,0,20,20,"
      expected_buffer += "10,10,10,0,0,0"
      expect_buffer("11:win:cyn:gry:2:cpy", 0, 12, expected_buffer, True, True)

    if test("it copies and duplicates separately"):
      expect_buffer("org:grn:flu:2,-1:cpy:era:flu:2,-2:cpy", 0, 3, "0,20,0,20,10,0,0,0,0")

    if test("duplicated pattern uses the palette buffer if it fits"):
      expect_palette("blu:wht:blk:flu:3,-1:cpy", 0, 3, "0,0,0,20,20,20,0,0,20")

    if test("duplicated pattern uses the render buffer if too big for the palette buffer"):
      expect_palette("1:rnd:" + str(palette_size) + ":rep:flu:" + str(palette_size + 1) + ",-1:cpy", 0, palette_size, standard_palette)

    if test("it pastes what's in the palette without copying"):
      expect_buffer(str(palette_size) + ",-2:cpy", 0, palette_size, standard_palette, True, True)

    if test("it pastes the pattern at the current offset"):
      expect_buffer("yel:olv:flu:2,-1:cpy:era:1:off:2,-2:cpy", 0, 4, "0,0,0,15,20,0,20,20,0,0,0,0") 

    if test("it duplicates any arbitrary pattern directly from the palette buffer"):
      expect_buffer(str(palette_size) + ",-2:cpy", 0, palette_size, standard_palette, True, True)

    if test("it copies the effects too when using palette memory"):
      expect_effect("wht:bre:1,2:cpy", 0, 3, "21,21,0")

    if test("it copies the effects too when using render memory"):
      expected_effect = ""
      for n in range(0, palette_size + 1):
        expected_effect += "21,"
      expected_effect += "0"
      expect_effect("dgr:bre:" + str(palette_size) + ":rep:flu:" + str(palette_size + 1) + ",1:cpy", 0, palette_size + 2, expected_effect)

    if test("effects are not set on a duplicate-only operation"):
      expect_effect("lgr:bl1:1,-1:cpy:flu:era:1,-2:cpy", 0, 1, "0")


########################################################################
# PALETTE SHUFFING
########################################################################
  if group("palette shuffling"):                                                            

    if test("the palette can be shuffled"):
      expect_palette("shf", 0, palette_size, standard_palette, False)
      expected_colors = "10,20,0,20,0,0,0,15,20,0,20,0,15,20,0,20,20,0,20,0,15,0,0,20,10,0,20,20,10,0,0,20,15,0,10,20,20,15,0,20,0,20,20,0,10,15,0,20,0,20,10,0,20,20"
      expect_palette("shf", 0, palette_size, expected_colors)

    if test("the palette resets to the right fixed set of colors"):
      expect_palette("shf:flu:1:shf", 0, palette_size, standard_palette)

    if lc.get_extra_shuffles_enabled():
      if test("the shuffler sets every odd-numbered palette color to the previous one's compliment"):
        expect_palette("2:shf", 0, palette_size, standard_palette, False)
        expected_colors = "0,20,20,20,0,0,0,0,20,20,20,0,0,10,20,20,10,0,5,20,0,15,0,20,20,10,0,0,10,20,0,20,10,20,0,10,20,15,0,0,5,20,0,15,20,20,5,0,15,0,20,5,20,0"
        expect_palette("shf:flu:2:shf", 0, palette_size, expected_colors)                                                                         

      if test("the shuffler creates a random palette of complimentary pairs"):
        expect_palette("3:shf", 0, palette_size, standard_palette, False)
        expected_colors = "0,10,20,20,10,0,0,15,20,20,5,0,20,10,0,0,10,20,0,20,20,20,0,0,20,0,10,0,20,10,0,20,15,20,0,5,20,15,0,0,5,20,10,20,0,10,0,20,0,20,10,20,0,10"
        expect_palette("flu:3:shf", 0, palette_size, expected_colors)                                                                 

      if test("the shuffler compliments the entire current palette"):
        expect_palette("4:shf", 0, palette_size, standard_palette, False)
        expected_colors = "0,20,20,0,10,20,0,0,20,20,0,20,20,20,0,10,20,0,20,0,0,0,20,0,20,10,0,10,0,20,20,0,10,0,20,10,0,5,20,5,0,20,20,5,0,20,0,5,5,20,0,0,20,5"
        expect_palette("1:shf:flu:4:shf", 0, palette_size, expected_colors)                                                                                             

      if test("the shuffler rotates the current palettes down"):
        expect_palette("5:shf", 0, palette_size, standard_palette, False)
        expected_colors = "20,10,0,20,20,0,0,20,0,0,0,20,10,0,20,0,20,20,20,0,20,0,10,20,10,20,0,0,20,10,20,0,10,20,15,0,15,20,0,0,15,20,0,20,15,15,0,20,20,0,15,20,0,0"
        expect_palette("1:shf:flu:5:shf", 0, palette_size, expected_colors)

      if test("the shuffler rotates the current palette up"):
        expect_palette("6:shf", 0, palette_size, standard_palette, False)
        expected_colors = "20,0,15,20,0,0,20,10,0,20,20,0,0,20,0,0,0,20,10,0,20,0,20,20,20,0,20,0,10,20,10,20,0,0,20,10,20,0,10,20,15,0,15,20,0,0,15,20,0,20,15,15,0,20"
        expect_palette("1:shf:flu:6:shf", 0, palette_size, expected_colors)

      if test("the shuffler rotates the palette down a number of times"):
        expected_colors = "20,20,0,0,20,0,0,0,20,10,0,20,0,20,20,20,0,20,0,10,20,10,20,0,0,20,10,20,0,10,20,15,0,15,20,0,0,15,20,0,20,15,15,0,20,20,0,15,20,0,0,20,10,0"
        expect_palette("1:shf:flu:5,2:shf", 0, palette_size, expected_colors)

      if test("the shuffler rotates the palette up a number of times"):
        expected_colors = "0,20,15,15,0,20,20,0,15,20,0,0,20,10,0,20,20,0,0,20,0,0,0,20,10,0,20,0,20,20,20,0,20,0,10,20,10,20,0,0,20,10,20,0,10,20,15,0,15,20,0,0,15,20"
        expect_palette("1:shf:flu:6,3:shf", 0, palette_size, expected_colors)

      if test("the shuffer rotates a number of positions of the palette down"):
        expected_colors = "20,10,0,20,20,0,0,20,0,20,0,0,0,0,20,10,0,20,0,20,20,20,0,20,0,10,20,10,20,0,0,20,10,20,0,10,20,15,0,15,20,0,0,15,20,0,20,15,15,0,20,20,0,15"
        expect_palette("1:shf:flu:5,0,4:shf", 0, palette_size, expected_colors)

      if test("the shuffler rotates a number of positions of the palette up"):
        expected_colors = "0,0,20,20,0,0,20,10,0,20,20,0,0,20,0,10,0,20,0,20,20,20,0,20,0,10,20,10,20,0,0,20,10,20,0,10,20,15,0,15,20,0,0,15,20,0,20,15,15,0,20,20,0,15"
        expect_palette("1:shf:flu:6,0,5:shf", 0, palette_size, expected_colors)

      if test("the shuffler reverses the current palette"):
        expect_palette("7:shf", 0, palette_size, standard_palette, False)
        expected_colors = "20,0,15,15,0,20,0,20,15,0,15,20,15,20,0,20,15,0,20,0,10,0,20,10,10,20,0,0,10,20,20,0,20,0,20,20,10,0,20,0,0,20,0,20,0,20,20,0,20,10,0,20,0,0"
        expect_palette("1:shf:flu:7:shf", 0, palette_size, expected_colors)
    else:
      skip_test("shf", "extra palette shuffling features not enabled on this device.", 10)

########################################################################
# ZONES
########################################################################
  if group("zones"):                                                                          

    if test("zone zero is the entire display"):
      expect_offset("0:zon:dgr:flo", 0)
      expect_window("0:zon:olv:flo", num_leds)

    if test("zone one is the first 'fine' zone and not equal to the whole display"):
      window = lc.get_window()
      expect_window("1:zon:lav:flo", window, False)
                                                                                                                                                                                                            
    if test("there are always at least two fine zones, and the second doesn't start at zero"):
      offset = lc.get_offset()
      window = lc.get_window()
      expect_offset("2:zon:amb:flo", offset, False)
      expect_offset("2:zon:ros:flo", window, False)


########################################################################
# OFFSET AND WINDOW
########################################################################
  if group("setting offset and window"):                                                                          
                                                              
    if test("an offset can be set"):
      expect_buffer("1:off:grn", 0, 2, "0,0,0,0,20,0")

    if test("a window can be set"):
      expect_buffer("2:win:neo:flo", 0, 2, "20,5,0,20,5,0")

    if test("pushed-off-the-end colors don't exceed the window boundary"):
      expect_buffer("2:win:lbl:flo:lbl", 0, 2, "0,10,20,0,10,20")

    if test("setting an offset is not relative to the current offset"):
      expect_buffer("1:off:1:off:lgr", 0, 2, "0,0,0,10,20,0")

    if test("on setting offset override, it adjusts the window override if set"):
      expect_window("2:win:4:off:dgr", 5)  

    if test("on setting window override, it adjusts the offset override if set"):
      expect_offset("6:off:4:win:dgr", 3)

    if test("on setting offset override, it doesn't adjust the window override if not set"):
      expect_window("4:off:dgr", num_leds)

    if test("on setting window override, it doesn't adjust the offset override if not set"):
      expect_offset("4:win:dgr", 0)

    if test("on setting offset less than zero, it should be set to zero"):
      expect_offset("-1:off", 0)

    if test("on setting offset = num_leds, it should be set to num leds - 1"):
      expect_offset(str(num_leds) + ":off", num_leds-1)

    if test("on setting offset > num_leds, it should be set to num leds - 1"):
      expect_offset(str(num_leds + 10) + ":off", num_leds-1)

    if test("on setting window = num leds, it should be set to num leds"):
      expect_window(str(num_leds) + ":win", num_leds)

    if test("on setting window > num leds, it should be set to num leds"):
      expect_window(str(num_leds + 10) + ":win", num_leds)

########################################################################
# REVERSE AND FORWARD
########################################################################
  if group("reverse and forward"):                                                                          
          
    if test("the insertion mode can be set to reverse"):
      expect_buffer("5:win:1:rev:blu", 0, 6, "0,0,0,0,0,0,0,0,0,0,0,0,0,0,20,0,0,0")

    if test("the insertion mode can be set to normal"):
      expect_buffer("5:win:0:rev:yel", 0, 6, "20,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0")


########################################################################
# PUSHING RGB COLORS
########################################################################
  if group("rgb color"):                                                                          

    color_value = 255
    unscaled_color_value_ = unscaled_color_value(color_value)
    rendered_color_value_ = rendered_color_value(unscaled_color_value_)

    # compute pre-rendered value for full-brightness pixel
    expected_rgb_color = rgb_string(unscaled_color_value_, unscaled_color_value_, unscaled_color_value_)

    if test("it unscales to the proper pre-rendered value"):
      expect_buffer("255,255,255:rgb", 0, 1, expected_rgb_color)

    # compute rendered value for recovered full-brightness pixel
    expected_render_value = rendered_color_value_
    expected_rgb_color = rgb_string(expected_render_value, expected_render_value, expected_render_value)

    if test("it renders the expected RGB value in the render buffer"):                                                                                                        
      # must render at default brightness to recover the original value
      expect_render(str(default_brightness) + ":lev:255,255,255:rgb", 0, 1, expected_rgb_color)                                                         

    if test("current brightness level doesn't affect unscaling calculation"):
      expect_render("1:lev:255,255,255:rgb:" + str(default_brightness) + ":lev", 0, 1, expected_rgb_color)                         
                                                                  

########################################################################
# PUSHING HSL COLORS
########################################################################
  if group("hsl color"):                                                                          

    color_value = 255                                                                         
    unscaled_color_value_ = unscaled_color_value(color_value)                                                                                                                                                
    rendered_color_value_ = rendered_color_value(unscaled_color_value_)                                                                                                                                      
                                                         
    if test("it sets the expected HSL value in the display buffer"):                                                                                                                                     
      expected_rgb_color = str(unscaled_color_value_) + ",0,0"
      expect_buffer("0,255,255:hsl", 0, 1, expected_rgb_color)

    if test("it renders the expected HSL value in the render buffer"):                                                                                                                                   
      expected_rgb_color = str(rendered_color_value_) + ",0,0"                                                                                                                                                 
      expect_render(str(default_brightness) + ":lev:0,255,255:hsl", 0, 1, expected_rgb_color)                                                                                                          
                
    if test("current brightness level doesn't affect unscaling calculation"):                                                                                                                                    
      expect_render("1:lev:0,255,255:hsl:" + str(default_brightness) + ":lev", 0, 1, expected_rgb_color)
                                                                                                                                    

########################################################################
# CUSTOM BLACK LEVEL
########################################################################
  if group("custom black level"):                                                                          
  
    if test("a custom black level can be set"):
      expect_buffer("10,20,30:sbl:blk", 0, 1, "10,20,30")

    if test("erases using the custom black level"):
      expect_buffer("2,3,4:sbl:era", 0, 1, "2,3,4")


########################################################################
# PUSHING RANDOM COLORS
########################################################################
  if group("random color"):                                                                          
                                                             
    if test("it chooses a random color"):
      expect_buffer("rnd", 0, 1, "15,20,0")  

    if test("it chooses another random color"):
      expect_buffer("rnd", 0, 1, "15,20,0")                                                                                                                                 

    if test("it sets no effect"):
      expect_effect("rnd", 0, 1, "0")

    if test("it repeats using the same color not another random color"):               
      expect_buffer("rnd:rep", 0, 2, "15,20,0,15,20,0")  

    if test("it floods using the same color not another random color"):
      expect_buffer("rnd:flo", 0, 2, "15,20,0,15,20,0")

    if test("it sets a random color and sets no effect"):
      expect_buffer("1:rnd", 0, 1, "15,20,0")                                                                                        
      expect_effect("1:rnd", 0, 1, "0")  

    if test("the flooded colors get no effect set"):                          
      expect_effect("1:rnd:flo", 0, 3, "0,0,0")    

    if test("it sets a random color and sets a random effect"):                                                                                                                          
      expect_buffer("1:rnd:2:rnd:2:rnd", 0, 3, "0,0,20,10,0,20,15,20,0")                                                                                                                                                                    
      expect_effect("1:rnd:2:rnd:2:rnd", 0, 3, "18,11,0") 

    if test("the flooded colors get the chosen effect"): 
      # extra commands added to skip over randomly-chosen no effect
      expect_buffer("2:rnd:2:rnd:flo", 0, 3, "20,20,0,20,20,0,20,20,0")
      expect_effect("2:rnd:2:rnd:flo", 0, 3, "11,11,11")

    if test("it places a random color from the palette"):
      expect_buffer("3:rnd", 0, 1, "15,20,0")

    if test("it places multiple versions of the same random color and no effect"):
      expect_buffer("rnd:0,5:rnd", 0, 7, "20,0,20,20,0,20,20,0,20,20,0,20,20,0,20,15,20,0,0,0,0")
      expect_effect("rnd:0,5:rnd", 0, 7, "0,0,0,0,0,0,0")

    if test("it places multiple random colors and no effects"):
      expect_buffer("rnd:1,5:rnd", 0, 7, "0,10,20,0,0,20,20,20,0,10,0,20,20,0,20,15,20,0,0,0,0")
      expect_effect("rnd:1,5:rnd", 0, 7, "0,0,0,0,0,0,0")

    if test("it places multiple random colors with random effects"):
      expect_buffer("rnd:2,5:rnd", 0, 7, "20,0,0,20,0,15,0,20,20,0,0,20,10,0,20,15,20,0,0,0,0")
      expect_effect("rnd:2,5:rnd", 0, 7, "15,18,21,17,19,0,13")

    if test("it places multiple random palette colors and no effects"):
      expect_buffer("rnd:3,5:rnd", 0, 7, "0,10,20,0,0,20,20,20,0,10,0,20,20,0,20,15,20,0,0,0,0")
      expect_effect("rnd:3,5:rnd", 0, 7, "0,0,0,0,0,0,0")

    if test("it fills the current width with random colors"):
      expect_buffer("3:win:1,-1:rnd", 0, 4, "10,0,20,20,0,20,15,20,0,0,0,0")

    if test("it fills the current width with random colors and effects"):
      expect_buffer("3:win:2,-1:rnd", 0, 4, "0,10,20,20,20,0,20,0,20,0,0,0")
      expect_effect("3:win:2,-1:rnd", 0, 4, "15,18,11,0")

    if test("it fills the current width with random palette colors and no effects"):
      expect_buffer("3:win:3,-1:rnd", 0, 4, "10,0,20,20,0,20,15,20,0,0,0,0")
      expect_effect("3:win:3,-1:rnd", 0, 4, "0,0,0,0")


########################################################################
# PUSHING PALETTE COLORS
########################################################################
  if group("palette color"):

    if test("it places the first palette color"):
      expect_buffer("pal", 0, 1, "20,0,0")

    if test("it places the second palette color"):
      expect_buffer("1:pal", 0, 1, "20,10,0")

    if test("it places multiple palette colors"):
      expect_buffer("0,1:pal", 0, 3, "20,0,0,20,10,0,0,0,0")

    if test("it places an alternate set of multiple palette colors"):
      expect_buffer("2,3:pal", 0, 3, "20,20,0,0,20,0,0,0,0")

    if test("it places all palette colors"):
      expect_buffer("0,17:pal", 0, 18, standard_palette, True, True) 


########################################################################
# BLENDING COLORS
########################################################################
  if group("blending colors"):                                                                          

    if lc.get_blend_enabled():
      # the color not in position 0 dominates the color blending

      if test("it blends two colors @ 50%"):
        expect_buffer("wht:blk:ble", 0, 3, "10,10,10,10,10,10,0,0,0")

      if test("it blends two colors @ 90%"):
        expect_buffer("wht:blk:90:ble", 0, 3, "2,2,2,2,2,2,0,0,0")

      if test("it blends two colors @ 10%"):
        expect_buffer("wht:blk:10:ble", 0, 3, "18,18,18,18,18,18,0,0,0")                                                                                                                                                                                                           
    else:
      skip_test("ble", "blend feature is not enabled on this device", 3)


########################################################################
# MAX, DIM AND BRIGHT
########################################################################
  if group("max, dim and bright"):                                                                          

    if test("it boosts the brightness level"):
      expect_buffer("wht:brt", 0, 1, "40,40,40")

    if test("it reduces the brightness level"):
      expect_buffer("wht:dim", 0, 1, "10,10,10")

    if test("it maxxes out the brightness level"):
      if default_brightness == 20:
        expect_buffer("wht:max", 0, 1, "204,204,204")                                                                                                                                                                                                           
      elif default_brightness == 25:
        expect_buffer("wht:max", 0, 1, "255,255,255")
      elif default_brightness == 10:
        expect_buffer("wht:max", 0, 1, "102,102,102")


########################################################################
# BLINK EFFECTS
########################################################################
  if group("blink effects"):                                                             

    if test("main blink effect"):

      # set the blink period to the minimum possible value 
      lc.command_str("0,6:cfg")

      # use a macro to process the effects and update the render buffer
      # this gets around the fact effects are reset on processing commands
      lc.command_str("0:set:6:tst:flu")

      # place a blinking red
      lc.command_str("red:bli")

      # simulate a half blink period
      # this will leave the render buffer in the dim/unblinked state
      expect_render("0,6:run", 0, 1, "2,0,0", False)

      # simulate a full blink period
      # this will leave the render buffer in the normal/blinked state
      expect_render("0,12:run", 0, 1, "51,0,0", False)

    if test("a/b blink effects"):
      lc.command_str("0,6:cfg")
      lc.command_str("0:set:6:tst:flu")

      # set one of each effect
      lc.command_str("grn:bla:blu:blb")

      # simulate a half blink period
      expect_render("0,3:run", 0, 2, "0,0,51,0,2,0", False)

      # simulate a full blink period
      expect_render("0,6:run", 0, 2, "0,0,2,0,51,0", False)

    if test("1/2/3/4/5/6 blink effects"):
      lc.command_str("0,6:cfg")
      lc.command_str("0:set:6:tst:flu")

      # set one of each effect
      lc.command_str("red:bl1:org:bl2:yel:bl3:grn:bl4:blu:bl5:pur:bl6")

      # simulate 1/6 blink period
      expect_render("0,1:run", 0, 6, "1,0,2,0,0,2,0,2,0,2,2,0,51,25,0,2,0,0", False)

      # simulate 2/6 blink period
      expect_render("0,2:run", 0, 6, "1,0,2,0,0,2,0,2,0,51,51,0,2,1,0,2,0,0", False)

      # simulate 3/6 blink period
      expect_render("0,3:run", 0, 6, "1,0,2,0,0,2,0,51,0,2,2,0,2,1,0,2,0,0", False)

      # simulate 4/6 blink period
      expect_render("0,4:run", 0, 6, "1,0,2,0,0,51,0,2,0,2,2,0,2,1,0,2,0,0", False)

      # simulate 5/6 blink period
      expect_render("0,5:run", 0, 6, "25,0,51,0,0,2,0,2,0,2,2,0,2,1,0,2,0,0", False)

      # simulate 6/6 blink period
      expect_render("0,6:run", 0, 6, "1,0,2,0,0,2,0,2,0,2,2,0,2,1,0,51,0,0", False)


########################################################################
# BREATHE EFFECT
########################################################################
  if group("breathe effect"):                                                             
                 
    if test("the breathe effect renders properly"):

      # set the breate period to the minimum possible value
      lc.command_str("1,1:cfg")

      # use a macro to process the effects and update the render buffer
      # this gets around the fact effects are reset on processing commands
      lc.command_str("0:set:6:tst:flu")

      # place a breathing greenn
      lc.command_str("grn:bre")

      # these are the expected values if using the floats for breathe ratio
      # expected_render_values = [ 0,  0,  0,  0,  0,  4,  8, 13, 17, 21, 25, 29, 32, 36, 39, 41, 44, 46, 47, 49, 50, 50, 
      #                           50, 49, 47, 46, 44, 41, 39, 36, 32, 29, 25, 21, 17, 13,  8,  4,  0,  0,  0,  0,  0,  0 ]

      # these are the expected values if using the bytes for breathe ratio
      expected_render_values = [ 0,  0,  0,  0,  4,  8, 13, 17, 21, 25, 29, 32, 36, 39, 41, 44, 46, 48, 49, 50, 50, 51,
                                50, 50, 49, 48, 46, 44, 41, 39, 36, 32, 29, 25, 21, 17, 13,  8,  4,  0,  0,  0,  0,  0 ]

      # simulate rendering through each breathe step period
      for n in range(0, len(expected_render_values)):
        expect_render("0," + str(n) + ":run", 0, 1, "0," + str(expected_render_values[n]) + ",0", False)


########################################################################
# FADING EFFECTS
########################################################################
  if group("fade effects"):                                                             
        
    if test("it modifies the display color with slow fades on flushing"):
      expect_buffer("red:sfd:flu", 0, 1, "19,0,0", False)
      expect_buffer("flu", 0, 1, "18,0,0", False)
      expect_buffer("flu", 0, 1, "17,0,0", False)

    if test("it renders the fading color with slow fades on flushing"):
      expect_render("red:sfd:flu", 0, 1, "48,0,0", False)
      expect_render("flu", 0, 1, "45,0,0", False)
      expect_render("flu", 0, 1, "43,0,0", False)

    if test("a custom slow fade rate modifies the display buffer properly"):
      lc.command_str("2,7500:cfg")
      expect_buffer("red:sfd:flu", 0, 1, "15,0,0", False)
      expect_buffer("flu", 0, 1, "11,0,0", False)
      expect_buffer("flu", 0, 1, "8,0,0", False)

    if test("a custom slow fade rate renders properly"):
      lc.command_str("2,7500:cfg")
      expect_render("red:sfd:flu", 0, 1, "38,0,0", False)
      expect_render("flu", 0, 1, "28,0,0", False)
      expect_render("flu", 0, 1, "20,0,0", False)

    if test("it modifies the display color with fast fades on flushing"):
      expect_buffer("red:ffd:flu", 0, 1, "10,0,0", False)
      expect_buffer("flu", 0, 1, "5,0,0", False)
      expect_buffer("flu", 0, 1, "2,0,0", False)

    if test("it renders the fading color with fast fades on flushing"):
      expect_render("red:ffd:flu", 0, 1, "25,0,0", False)
      expect_render("flu", 0, 1, "12,0,0", False)
      expect_render("flu", 0, 1, "5,0,0", False)


########################################################################
# RESET CLEAR AND STOP
########################################################################
  if group("reset, clear and stop"):                                                             
    pending_test("reset, clear and stop")

                                                                                                                                                                                                         
########################################################################
# BRIGHTNESS LEVEL
########################################################################
  if group("brightness level"):                                                             

    if test("it renders at a brightness level"):
      expect_render("5:lev:sea:flu", 0, 1, "0,12,6")

    if test("it renders at an alternate brightness level"):
      expect_render("35:lev:sea:flu", 0, 1, "0,89,44")

    if test("it renders at an automatic brightness level"):
      expect_render("0:lev:sea:flu", 0, 1, "0,38,19")

    if test("it renders at a high brightness level"):
      expect_render("-2:lev:sea:flu", 0, 1, "0,63,31")

    if test("it renders at a low brightness level"):
      expect_render("-3:lev:sea:flu", 0, 1, "0,12,6")

    if test("it renders at a max brightness level"):
      expect_render("-1:lev:sea:flu", 0, 1, "0,153,76")

    if test("it renders at a min brightness level"):
      expect_render("-4:lev:sea:flu", 0, 1, "0,5,2")

    if test("it renders at a 0 percent mapped brightness level"):
      expect_render("-5,0:lev:sea:flu", 0, 1, "0,0,0")

    if test("it renders at a 100 percent mapped brightness level"):
      expect_render("-5,100:lev:sea:flu", 0, 1, "0,153,76")


# FADE ANIMATION
########################################################################
  if group("fade animation"):                                                             

    if test("it leaves the buffer empty (black) after done"):
      expect_empty_buffer("amb:flo:flu:fad", 0, num_leds)                                                                                                                                                                                                           

    if test("it leaves the display empty (black) after done"):
      expect_empty_render("olv:flo:flu:fad", 0, num_leds)


########################################################################
# ANIMATED ROTATION
########################################################################
  if group("animated rotation"):

    if test("animated rotation"):
     expect_render("lbl:art", 0, 2, "0,0,0,0,25,51", False)

########################################################################
# ROTATION
########################################################################
  if group("rotation"):                                                             

    if test("it rotates within the current window"):
      expect_buffer("0:off:5:win:red:rot", 0, 5, "0,0,0,20,0,0,0,0,0,0,0,0,0,0,0")

    if test("it rotates in reverse in the current window"):
      expect_buffer("0:off:5:win:blu:1:rev:rot", 0, 5, "0,0,0,0,0,0,0,0,0,0,0,0,0,0,20")

    if test("it rotates multiple times within the current window"):
      expect_buffer("0:off:5:win:red:2:rot", 0, 5, "0,0,0,0,0,0,20,0,0,0,0,0,0,0,0")

    if test("it rotates multiple times in reverse in the current window"):
      expect_buffer("0:off:5:win:blu:1:rev:2:rot", 0, 5, "0,0,0,0,0,0,0,0,0,0,0,20,0,0,0")                                                                                                                                                                                                           

    if test("it carries the correct color to the insertion point"):
      expect_buffer("0:off:10:win:red:grn:blu:6:blk:wht:rot", 0, 10, "20,0,0,20,20,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,20,0,20,0")

    if test("it carries the correct color to the insertion point in reverse"):
      expect_buffer("0:off:10:win:1:rev:red:grn:blu:6:blk:wht:rot", 0, 10, "0,20,0,0,0,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,20,20,20,20,0,0")


########################################################################
# POWER SHIFT
########################################################################
#  group("power shift")                                                             
                                                                                                                                                                                                           

########################################################################
# CROSSFADE ANIMATION
########################################################################
#  group("crossfade")                                                             
                                                                                                                                                                                                           

########################################################################
# SETTING BLINK PERIOD
########################################################################
  if group("setting blink period"):                                                             

    if test("a custom blink period can be set"):                                                                                                                                                                                                           

      # use a macro to process the effects and update the render buffer
      # this gets around the fact effects are reset on processing commands
      lc.command_str("0:set:6:tst:flu")

      # place a blinking orange
      lc.command_str("org:bli")

      # set the blink period to the minimum possible value
      lc.command_str("0,6:cfg")

      # the main blink is on for the first half of the cycle
      # this will start the second half and leave the render buffer in the dim/unblinked state
      expect_render("0,6:run", 0, 1, "2,1,0", False)

      # this will advance it back to the start of the first cycle
      # this will leave the render buffer in the normal/blinked state
      expect_render("0,12:run", 0, 1, "51,25,0", False)

      # this will advance it to the second half of the second cycle
      # this will leave the render buffer in the dim/unblinked state
      expect_render("0,18:run", 0, 1, "2,1,0", False)

      # this will advance it to the first  half of the third cycle
      # this will leave the render buffer in the dim/unblinked state
      expect_render("0,24:run", 0, 1, "51,25,0", False)

    if test("a different custom blink period can be set"):

      # use a macro to process the effects and update the render buffer
      # this gets around the fact effects are reset on processing commands
      lc.command_str("0:set:6:tst:flu")

      # place a blinking rose
      lc.command_str("ros:bli")

      # set the blink period to the twice the previous value
      lc.command_str("0,12:cfg")

      # the main blink is on for the first half of the cycle
      # simulate a quarter blink period
      # this will leave the render buffer in the normal/blinked state
      expect_render("0,6:run", 0, 1, "51,0,38", False)

      # simulate a half blink period
      # this will leave the render buffer in the dim/unblinked state
      expect_render("0,12:run", 0, 1, "2,0,1", False)

      # simulate a three quarter blink period
      # this will leave the render buffer in the dim/unblinked state
      expect_render("0,18:run", 0, 1, "2,0,1", False)

      # simulate a full blink period
      # this will leave the render buffer in the normal/blinked state
      expect_render("0,24:run", 0, 1, "51,0,38", False)


########################################################################
# CARRY COLOR
########################################################################
  if group("carry color"):                                                                                                            

    skip_test("2:win:red:blu:flu:rot:flu:car", "carry color")                                                                                                                                                                                                           
    # expect_buffer("2:win:red:blu:flu:rot:flu:car", 0, 2, "20,0,0,20,0,0")


########################################################################
# CUSTOM BREATHE TIME
########################################################################
  if group("setting custom breathe time"):                                                                                                            

    if test("setting a custom breathe time"):                                                                                                                                                                                                           

      # set the breate period to the minimum possible value
      lc.command_str("1,1:cfg")

      # use a macro to process the effects and update the render buffer
      # this gets around the fact effects are reset on processing commands
      lc.command_str("0:set:6:tst:flu")

      # place a breathing blue
      lc.command_str("blu:bre")

      # these are the expected values if using the floats for breathe ratio
      # expected_render_values = [ 0,  0,  0,  0,  0,  4,  8, 13, 17, 21, 25, 29, 32, 36, 39, 41, 44, 46, 47, 49, 50, 50,
      #                           50, 49, 47, 46, 44, 41, 39, 36, 32, 29, 25, 21, 17, 13,  8,  4,  0,  0,  0,  0,  0,  0 ]

      # these are the expected values if using the bytes for breathe ratio
      # expected_render_values = [ 0,  0,  0,  0,  0,  4,  8, 13, 17, 21, 25, 29, 32, 36, 39, 41, 44, 46, 48, 49, 50, 50,
      #                           50, 49, 48, 46, 44, 41, 39, 36, 32, 29, 25, 21, 17, 13,  8,  4,  0,  0,  0,  0,  0,  0 ]

      # simulate rendering through each breathe step period
      # for n in range(0, len(expected_render_values)):
      #   expect_render("0," + str(n) + ":run", 0, 1, "0," + str(expected_render_values[n]) + ",0", False)

      expect_render("0,10:run", 0, 1, "0,0,29", False)
      expect_render("0,11:run", 0, 1, "0,0,32", False)
      expect_render("0,12:run", 0, 1, "0,0,36", False)
      expect_render("0,13:run", 0, 1, "0,0,39", False)

      test("setting an alternate custom breathe time")

      # set the breathe time to twice the previous value
      lc.command_str("1,2:cfg")

      # use a macro to process the effects and update the render buffer
      # this gets around the fact effects are reset on processing commands
      lc.command_str("0:set:6:tst:flu")

      # place a breathing blue
      lc.command_str("blu:bre")

      expect_render("0,10:run", 0, 1, "0,0,8", False)
      expect_render("0,11:run", 0, 1, "0,0,8", False)
      expect_render("0,12:run", 0, 1, "0,0,13", False)
      expect_render("0,13:run", 0, 1, "0,0,13", False)


########################################################################
# COMMAND PROCESSING BUG FIX
########################################################################
  if group("test fix for 50-59 argument parsing"):

   if test("a known bug is fixed - using values 50-59 as arguments in setting macros uses too many bytes"):
     for x in range(49,61):
       expect_accumulators("rnd:" + str(x) + ",1:sto", str(x) + ",1,0")


########################################################################
# MACROS
########################################################################
  if group("setting and running macros"):                                                                                                            

    if test("a macro can be set"):
      lc.command_str("0:set:red:wht:blu")
      expect_buffer("0:run", 0, 3, "0,0,20,20,20,20,20,0,0")

    if test("a macro can be set from within another macro"):
      lc.command_str("0:set:1:set:olv:amb:lav")
      expect_buffer("0:run:1:run", 0, 3, "15,0,20,20,15,0,15,20,0")

    if test("a macro can be set from within another macro that was set from within another macro"):
      lc.command_str("0:set:1:set:2:set:cyn:yel:mag")
      expect_buffer("0:run:1:run:2:run", 0, 3, "20,0,20,20,20,0,0,20,20")

    pending_test("more general macro tests")


########################################################################
# DELAY
########################################################################
  if group("delay"):                                                                                                            

    pending_test("delay")                                                                                                                                                                                                           


########################################################################
# RANDOM NUMBER
########################################################################
  if group("random number"):                                                                                                            

    pending_test("random numnber")                                                                                                                                                                                                           


########################################################################
# POSITION
########################################################################
  if group("position"):                                                                                                            

    pending_test("position")                                                                                                                                                                                                           


########################################################################
# RANDOM POSITION
########################################################################
  if group("random postion"):                                                                                                            

    if test("it sets a random position within the current width"):                                                                                                                                                                                                         

      # use a macro to process the ramndom postion and place a color
      lc.command_str("0:set:5:win:rps:rnd:flu:rst")    

      expect_buffer("0,20:run", 0, 10, "10,20,0,20,0,20,20,10,0,20,10,0,0,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0")

      ## the test region starts at zero because when it doesn't find a position it returns zero

    if test("it sets a random position only where empty"):
      # use a macro to process the ramndom postion and place a color
      lc.command_str("0:set:4:win:-1:rps:wht:flu:rst")

      expect_buffer("2:dgr:2:blk:0,2:run", 0, 5, "20,20,20,20,20,20,5,5,5,5,5,5,0,0,0")

    if test("it sets a random position only where not empty"):

      # use a macro to process the ramndom postion and place a color
      lc.command_str("0:set:4:win:-2:rps:wht:flu:rst")

      expect_buffer("2:blk:2:dgr:0,10:run", 0, 5, "20,20,20,20,20,20,0,0,0,0,0,0,0,0,0")

    if test("doesn't get stuck if there are no empty spots"):
      expect_buffer("2:win:pnk:flo:-1:rps:wht", 0, 2, "20,20,20,20,0,10")

    if test("doesn't get stuck if there are no non-empty spots"):
      expect_buffer("2:win:-2:rps:wht", 0, 2, "20,20,20,0,0,0")


########################################################################
# SEQUENCING
########################################################################
  if group("sequencing"):                                                                                                            
    if test("setting a sequence leaves arg0 set to the low value"):
      expect_buffer("0,5,4:seq:olv", 0, 5, "15,20,0,15,20,0,15,20,0,15,20,0,0,0,0")
                                                                                                                                                                                                         
    if test("it does a wheel sequence with only an upper limit"):
      expect_sequence("0,5:seq:sto:red", "0:seq:sto:red", [0,1,2,3,4,0,1,2,3,4,0])

    if test("it does a wheel sequence with an upper and lower limit"):
      expect_sequence("0,5,1:seq:sto:red", "0:seq:sto:red", [1,2,3,4,1,2,3,4,1])

    if test("wheel sequence handles negative low limit"):
      expect_sequence("0,5,-5:seq:sto:red", "0:seq:sto:red", [-5,-4,-3,-2,-1,0,1,2,3,4,-5,-4])

    if test("wheel sequence handles > 8-bit values"):
      expect_sequence("0,1000:seq:sto", "0,0,100:seq:sto", [0,100,200,300,400,500,600,700,800,900,0,100,200,300,400,500,600,700,800,900,0])

    if test("wheel sequence handles > 8-bit values #2"):
      expect_sequence("0,10000:seq:sto", "0,0,1000:seq:sto", [0,1000,2000,3000,4000,5000,6000,7000,8000,9000,0,1000,2000,3000,4000,5000,6000,7000,8000,9000,0])

    if test("wheel sequence handles > 8-bit values #3"):
      expect_sequence("0,65535:seq:sto", "0,0,4096:seq:sto", [0,1000,2000,3000,4000,5000,6000,7000,8000,9000,0,1000,2000,3000,4000,5000,6000,7000,8000,9000,0])


    if test("swing sequence handles negative low limit"):
      expect_sequence("0,5,-5:sqs:sto:blu", "0:seq:sto:blu", [-5,-4,-3,-2,-1,0,1,2,3,4,3,2,1,0,-1,-2,-3,-4,-5,-4])


    if test("it does a swing sequence with only an upper limit"):
      expect_sequence("0,5:sqs:sto:org", "0:seq:sto:org", [0, 1, 2, 3, 4, 3, 2, 1, 0, 1])

    if test("it does a swing sequence with an upper and lower limit"):
      expect_sequence("0,5,1:sqs:sto:org", "0:seq:sto:org", [1,2,3,4,3,2,1,2])

    if test("it does a wheel cosine sequence"):
      expect_buffer("0,7,1:swc:red:flu", 0, 2, "20,0,0,0,0,0", True, True)
      expect_buffer("seq:org", 0, 5,       "20,10,0,20,10,0,20,10,0,20,0,0,0,0,0", True, True)
      expect_buffer("seq:yel", 0, 11,      "20,20,0,20,20,0,20,20,0,20,20,0,20,20,0,20,20,0,20,10,0,20,10,0,20,10,0,20,0,0,0,0,0", True, True)
      expect_buffer("seq:grn", 0, 17,      "0,20,0,0,20,0,0,20,0,0,20,0,0,20,0,0,20,0,20,20,0,20,20,0,20,20,0,20,20,0,20,20,0,20,20,0,20,10,0,20,10,0,20,10,0,20,0,0,0,0,0", True, True)
      expect_buffer("seq:blu", 0, 20,      "0,0,20,0,0,20,0,0,20,0,20,0,0,20,0,0,20,0,0,20,0,0,20,0,0,20,0,20,20,0,20,20,0,20,20,0,20,20,0,20,20,0,20,20,0,20,10,0,20,10,0,20,10,0,20,0,0,0,0,0", True, True)
      expect_buffer("seq:pur", 0, 21,      "10,0,20,0,0,20,0,0,20,0,0,20,0,20,0,0,20,0,0,20,0,0,20,0,0,20,0,0,20,0,20,20,0,20,20,0,20,20,0,20,20,0,20,20,0,20,20,0,20,10,0,20,10,0,20,10,0,20,0,0,0,0,0", True, True)
      expect_buffer("seq:red", 0, 22,      "20,0,0,10,0,20,0,0,20,0,0,20,0,0,20,0,20,0,0,20,0,0,20,0,0,20,0,0,20,0,0,20,0,20,20,0,20,20,0,20,20,0,20,20,0,20,20,0,20,20,0,20,10,0,20,10,0,20,10,0,20,0,0,0,0,0", True, True)

    if test("it does a wheel sine sequence"):
      expect_buffer("0,7,1:sws:red:flu", 0, 2, "20,0,0,0,0,0", True, True)
      expect_buffer("seq:org", 0, 3,       "20,10,0,20,0,0,0,0,0", True, True)
      expect_buffer("seq:yel", 0, 5,       "20,20,0,20,20,0,20,10,0,20,0,0,0,0,0", True, True)
      expect_buffer("seq:grn", 0, 10,      "0,20,0,0,20,0,0,20,0,0,20,0,0,20,0,20,20,0,20,20,0,20,10,0,20,0,0,0,0,0", True, True)
      expect_buffer("seq:blu", 0, 16,      "0,0,20,0,0,20,0,0,20,0,0,20,0,0,20,0,0,20,0,20,0,0,20,0,0,20,0,0,20,0,0,20,0,20,20,0,20,20,0,20,10,0,20,0,0,0,0,0", True, True)
      expect_buffer("seq:pur", 0, 19,      "10,0,20,10,0,20,10,0,20,0,0,20,0,0,20,0,0,20,0,0,20,0,0,20,0,0,20,0,20,0,0,20,0,0,20,0,0,20,0,0,20,0,20,20,0,20,20,0,20,10,0,20,0,0,0,0,0", True, True)
      expect_buffer("seq:red", 0, 22,      "20,0,0,20,0,0,20,0,0,10,0,20,10,0,20,10,0,20,0,0,20,0,0,20,0,0,20,0,0,20,0,0,20,0,0,20,0,20,0,0,20,0,0,20,0,0,20,0,0,20,0,20,20,0,20,20,0,20,10,0,20,0,0,0,0,0", True, True)

    if test("it does an opposite sequence"):
      expect_buffer("0,7,1:seq:red:flu", 0, 2, "20,0,0,0,0,0", True, True)
      expect_buffer("0,-3:seq:org", 0, 7,       "20,10,0,20,10,0,20,10,0,20,10,0,20,10,0,20,0,0,0,0,0", True, True)
      expect_buffer("0,-3:seq:yel", 0, 11,      "20,20,0,20,20,0,20,20,0,20,20,0,20,10,0,20,10,0,20,10,0,20,10,0,20,10,0,20,0,0,0,0,0", True, True)
      expect_buffer("0,-3:seq:grn", 0, 14,      "0,20,0,0,20,0,0,20,0,20,20,0,20,20,0,20,20,0,20,20,0,20,10,0,20,10,0,20,10,0,20,10,0,20,10,0,20,0,0,0,0,0", True, True)
      expect_buffer("0,-3:seq:blu", 0, 16,      "0,0,20,0,0,20,0,20,0,0,20,0,0,20,0,20,20,0,20,20,0,20,20,0,20,20,0,20,10,0,20,10,0,20,10,0,20,10,0,20,10,0,20,0,0,0,0,0", True, True)
      expect_buffer("0,-3:seq:pur", 0, 17,      "10,0,20,0,0,20,0,0,20,0,20,0,0,20,0,0,20,0,20,20,0,20,20,0,20,20,0,20,20,0,20,10,0,20,10,0,20,10,0,20,10,0,20,10,0,20,0,0,0,0,0", True, True)
      expect_buffer("0,-3:seq:red", 0, 23,      "20,0,0,20,0,0,20,0,0,20,0,0,20,0,0,20,0,0,10,0,20,0,0,20,0,0,20,0,20,0,0,20,0,0,20,0,20,20,0,20,20,0,20,20,0,20,20,0,20,10,0,20,10,0,20,10,0,20,10,0,20,10,0,20,0,0,0,0,0", True, True)

    if test("it does an opposite swing sequence"):
      expect_buffer("0,7,1:sqs:red:flu", 0, 2, "20,0,0,0,0,0", True, True)
      expect_buffer("0,-3:seq:org", 0, 7,       "20,10,0,20,10,0,20,10,0,20,10,0,20,10,0,20,0,0,0,0,0", True, True)
      expect_buffer("0,-3:seq:yel", 0, 11,      "20,20,0,20,20,0,20,20,0,20,20,0,20,10,0,20,10,0,20,10,0,20,10,0,20,10,0,20,0,0,0,0,0", True, True)
      expect_buffer("0,-3:seq:grn", 0, 14,      "0,20,0,0,20,0,0,20,0,20,20,0,20,20,0,20,20,0,20,20,0,20,10,0,20,10,0,20,10,0,20,10,0,20,10,0,20,0,0,0,0,0", True, True)
      expect_buffer("0,-3:seq:blu", 0, 16,      "0,0,20,0,0,20,0,20,0,0,20,0,0,20,0,20,20,0,20,20,0,20,20,0,20,20,0,20,10,0,20,10,0,20,10,0,20,10,0,20,10,0,20,0,0,0,0,0", True, True)
      expect_buffer("0,-3:seq:pur", 0, 17,      "10,0,20,0,0,20,0,0,20,0,20,0,0,20,0,0,20,0,20,20,0,20,20,0,20,20,0,20,20,0,20,10,0,20,10,0,20,10,0,20,10,0,20,10,0,20,0,0,0,0,0", True, True)
      expect_buffer("0,-3:seq:red", 0, 19,      "20,0,0,20,0,0,10,0,20,0,0,20,0,0,20,0,20,0,0,20,0,0,20,0,20,20,0,20,20,0,20,20,0,20,20,0,20,10,0,20,10,0,20,10,0,20,10,0,20,10,0,20,0,0,0,0,0", True, True)

# this doesn't work; doesn't fit the architecture - needs to draw two segments, partially before and after the seam.
#    if test("it steps a wheel sequence by -1"):
#      expect_buffer("0,7,1:seq:red:flu", 0, 2, "20,0,0,0,0,0", True, True)
#      expect_buffer("0,0,-1:seq:org", 0, 3,       "20,10,0,20,0,0,0,0,0", True, True)
#      expect_buffer("0,0,-1:seq:yel", 0, 8,       "20,20,0,20,20,0,20,20,0,20,20,0,20,20,0,20,20,0,20,20,0,0,0,0", True, True)
#      expect_buffer("0,0,-1:seq:grn", 0, 12,      "0,20,0,0,20,0,0,20,0,0,20,0,0,20,0,0,20,0,0,20,0,0,20,0,0,20,0,0,20,0,0,20,0,0,0,0", True, True)
#      expect_buffer("0,0,-1:seq:blu", 0, 17,      "0,0,20,0,0,20,0,0,20,0,0,20,0,0,20,0,0,20,0,0,20,0,0,20,0,0,20,0,0,20,0,0,20,0,0,20,0,0,20,0,0,20,0,0,20,0,0,20,0,0,0", True, True)
#      expect_buffer("0,0,-1:seq:pur", 0, 23,      "10,0,20,10,0,20,10,0,20,10,0,20,10,0,20,10,0,20,10,0,20,10,0,20,10,0,20,10,0,20,10,0,20,10,0,20,10,0,20,10,0,20,10,0,20,10,0,20,10,0,20,10,0,20,10,0,20,10,0,20,10,0,20,10,0,20,0,0,0", True, True)
#      expect_buffer("0,0,-1:seq:red", 0, 24,      "20,0,0,20,0,0,20,0,0,20,0,0,20,0,0,20,0,0,20,0,0,20,0,0,20,0,0,20,0,0,20,0,0,20,0,0,20,0,0,20,0,0,20,0,0,20,0,0,20,0,0,20,0,0,20,0,0,20,0,0,20,0,0,20,0,0,20,0,0,0,0,0", True, True)

#    if test("it steps a swing sequence by -1"):
#      expect_buffer("0,7,1:sqs:red:flu", 0, 2, "20,0,0,0,0,0", True, True)
#      expect_buffer("0,0,-1:seq:org", 0, 3,       "20,10,0,20,0,0,0,0,0", True, True)
#      expect_buffer("0,0,-1:seq:yel", 0, 8,       "20,20,0,20,20,0,20,20,0,20,20,0,20,20,0,20,20,0,20,20,0,0,0,0", True, True)
#      expect_buffer("0,0,-1:seq:grn", 0, 12,      "0,20,0,0,20,0,0,20,0,0,20,0,0,20,0,0,20,0,0,20,0,0,20,0,0,20,0,0,20,0,0,20,0,0,0,0", True, True)
#      expect_buffer("0,0,-1:seq:blu", 0, 17,      "0,0,20,0,0,20,0,0,20,0,0,20,0,0,20,0,0,20,0,0,20,0,0,20,0,0,20,0,0,20,0,0,20,0,0,20,0,0,20,0,0,20,0,0,20,0,0,20,0,0,0", True, True)
#      expect_buffer("0,0,-1:seq:pur", 0, 23,      "10,0,20,10,0,20,10,0,20,10,0,20,10,0,20,10,0,20,10,0,20,10,0,20,10,0,20,10,0,20,10,0,20,10,0,20,10,0,20,10,0,20,10,0,20,10,0,20,10,0,20,10,0,20,10,0,20,10,0,20,10,0,20,10,0,20,0,0,0", True, True)
#      expect_buffer("0,0,-1:seq:red", 0, 23,      "20,0,0,10,0,20,10,0,20,10,0,20,10,0,20,10,0,20,10,0,20,0,0,20,0,0,20,0,0,20,0,0,20,0,0,20,0,20,0,0,20,0,0,20,0,0,20,0,20,20,0,20,20,0,20,20,0,20,10,0,20,10,0,20,0,0,0,0,0", True, True)

    # test adjusting sequence high/low, fixing current 

    if test("the high limit can be changed"):
      expect_buffer("0,7,1:seq:red", 0, 2, "20,0,0,0,0,0", True, True)
      expect_buffer("seq:org", 0, 4,       "20,10,0,20,10,0,20,0,0,0,0,0", True, True)
      expect_buffer("seq:yel", 0, 7,       "20,20,0,20,20,0,20,20,0,20,10,0,20,10,0,20,0,0,0,0,0", True, True)
      expect_buffer("seq:grn", 0, 11,      "0,20,0,0,20,0,0,20,0,0,20,0,20,20,0,20,20,0,20,20,0,20,10,0,20,10,0,20,0,0,0,0,0", True, True)
      expect_buffer("seq:blu", 0, 16,      "0,0,20,0,0,20,0,0,20,0,0,20,0,0,20,0,20,0,0,20,0,0,20,0,0,20,0,20,20,0,20,20,0,20,20,0,20,10,0,20,10,0,20,0,0,0,0,0", True, True)
      expect_buffer("seq:pur", 0, 22,      "10,0,20,10,0,20,10,0,20,10,0,20,10,0,20,10,0,20,0,0,20,0,0,20,0,0,20,0,0,20,0,0,20,0,20,0,0,20,0,0,20,0,0,20,0,20,20,0,20,20,0,20,20,0,20,10,0,20,10,0,20,0,0,0,0,0", True, True)
      expect_buffer("seq:red", 0, 23,      "20,0,0,10,0,20,10,0,20,10,0,20,10,0,20,10,0,20,10,0,20,0,0,20,0,0,20,0,0,20,0,0,20,0,0,20,0,20,0,0,20,0,0,20,0,0,20,0,20,20,0,20,20,0,20,20,0,20,10,0,20,10,0,20,0,0,0,0,0", True, True)

      expect_buffer("era:flu:0,-7,4:seq:red", 0, 2, "20,0,0,0,0,0", True, True)
      expect_buffer("seq:org", 0, 4,       "20,10,0,20,10,0,20,0,0,0,0,0", True, True)
      expect_buffer("seq:yel", 0, 7,       "20,20,0,20,20,0,20,20,0,20,10,0,20,10,0,20,0,0,0,0,0", True, True)
      expect_buffer("seq:grn", 0, 8,       "0,20,0,20,20,0,20,20,0,20,20,0,20,10,0,20,10,0,20,0,0,0,0,0", True, True)

    if test("the low limit can be changed"):
      expect_buffer("0,7,1:seq:red", 0, 2, "20,0,0,0,0,0", True, True)
      expect_buffer("seq:org", 0, 4,       "20,10,0,20,10,0,20,0,0,0,0,0", True, True)
      expect_buffer("seq:yel", 0, 7,       "20,20,0,20,20,0,20,20,0,20,10,0,20,10,0,20,0,0,0,0,0", True, True)
      expect_buffer("seq:grn", 0, 11,      "0,20,0,0,20,0,0,20,0,0,20,0,20,20,0,20,20,0,20,20,0,20,10,0,20,10,0,20,0,0,0,0,0", True, True)
      expect_buffer("seq:blu", 0, 16,      "0,0,20,0,0,20,0,0,20,0,0,20,0,0,20,0,20,0,0,20,0,0,20,0,0,20,0,20,20,0,20,20,0,20,20,0,20,10,0,20,10,0,20,0,0,0,0,0", True, True)
      expect_buffer("seq:pur", 0, 22,      "10,0,20,10,0,20,10,0,20,10,0,20,10,0,20,10,0,20,0,0,20,0,0,20,0,0,20,0,0,20,0,0,20,0,20,0,0,20,0,0,20,0,0,20,0,20,20,0,20,20,0,20,20,0,20,10,0,20,10,0,20,0,0,0,0,0", True, True)
      expect_buffer("seq:red", 0, 23,      "20,0,0,10,0,20,10,0,20,10,0,20,10,0,20,10,0,20,10,0,20,0,0,20,0,0,20,0,0,20,0,0,20,0,0,20,0,20,0,0,20,0,0,20,0,0,20,0,20,20,0,20,20,0,20,20,0,20,10,0,20,10,0,20,0,0,0,0,0", True, True)

      expect_buffer("era:flu:0,-8,3:seq:red", 0, 4, "20,0,0,20,0,0,20,0,0,0,0,0", True, True)
      expect_buffer("seq:org", 0, 8,       "20,10,0,20,10,0,20,10,0,20,10,0,20,0,0,20,0,0,20,0,0,0,0,0", True, True)
      expect_buffer("seq:yel", 0, 13,      "20,20,0,20,20,0,20,20,0,20,20,0,20,20,0,20,10,0,20,10,0,20,10,0,20,10,0,20,0,0,20,0,0,20,0,0,0,0,0", True, True)
      expect_buffer("seq:grn", 0, 19,      "0,20,0,0,20,0,0,20,0,0,20,0,0,20,0,0,20,0,20,20,0,20,20,0,20,20,0,20,20,0,20,20,0,20,10,0,20,10,0,20,10,0,20,10,0,20,0,0,20,0,0,20,0,0,0,0,0", True, True)
      expect_buffer("seq:blu", 0, 22,      "0,0,20,0,0,20,0,0,20,0,20,0,0,20,0,0,20,0,0,20,0,0,20,0,0,20,0,20,20,0,20,20,0,20,20,0,20,20,0,20,20,0,20,10,0,20,10,0,20,10,0,20,10,0,20,0,0,20,0,0,20,0,0,0,0,0", True, True)

    if test("the sequencer can be reset"):
      expect_buffer("0,7,1:seq:red", 0, 2, "20,0,0,0,0,0", True, True)
      expect_buffer("seq:org", 0, 4,       "20,10,0,20,10,0,20,0,0,0,0,0", True, True)
      expect_buffer("seq:yel", 0, 7,       "20,20,0,20,20,0,20,20,0,20,10,0,20,10,0,20,0,0,0,0,0", True, True)
      expect_buffer("0,-9:seq:grn", 0, 8,  "0,20,0,20,20,0,20,20,0,20,20,0,20,10,0,20,10,0,20,0,0,0,0,0", True, True)

    if test("wheel sequencer stores index value as computed value"):
      expect_buffer("0,7,1:seq:0:seq:red", 0, 3, "20,0,0,20,0,0,0,0,0", True, True)      
      expect_buffer("0,-2:seq:org", 0, 5, "20,10,0,20,10,0,20,0,0,20,0,0,0,0,0", True, True)

    if test("swing sequencer stores index value as computed value"):
      expect_buffer("0,3,1:sqs:0:seq:red", 0, 3, "20,0,0,20,0,0,0,0,0", True, True)
      expect_buffer("0,-2:seq:org", 0, 5, "20,10,0,20,10,0,20,0,0,20,0,0,0,0,0", True, True)
      expect_buffer("0:seq:0,-2:seq:yel", 0, 6, "20,20,0,20,10,0,20,10,0,20,0,0,20,0,0,0,0,0", True, True)
      expect_buffer("0:seq:0,-2:seq:grn", 0, 8, "0,20,0,0,20,0,20,20,0,20,10,0,20,10,0,20,0,0,20,0,0,0,0,0", True, True)
      expect_buffer("0:seq:0,-2:seq:blu", 0, 9, "0,0,20,0,20,0,0,20,0,20,20,0,20,10,0,20,10,0,20,0,0,20,0,0,0,0,0", True, True)

    if test("two sequencers can be added"):
      expect_buffer("0,4,1:seq:1,4,1:seq:0,-5,1:seq:red", 0, 3, "20,0,0,20,0,0,0,0,0", True, True)

    if test("two sequencers can be subtracted"):
      expect_buffer("0,5,3:seq:1,3,1:seq:0,-6,1:seq:red", 0, 3, "20,0,0,20,0,0,0,0,0", True, True)




# steps, negative step
# next window
# marcos, resetting

########################################################################
# TESTING
########################################################################
  if group("testing"):                                                                                                            

    pending_test("test testing")


########################################################################
# PALETTE COLOR SWEEPING
########################################################################
  if group("palette color sweeping"):     

    if test("it sweeps the right default hues"):                                                                                  
      expected_colors = "20,0,0,20,6,0,20,13,0,20,20,0,13,20,0,6,20,0,0,20,0,0,20,6,0,20,13,0,20,20,0,13,20,0,6,20,0,0,20,6,0,20,13,0,20,20,0,20,20,0,13,20,0,6"    
      expect_palette("csh", 0, palette_size, expected_colors)                                                                                                 

    if test("it sweeps hues with a custom step angle"):                                                                                                                  
      expected_colors = "20,0,0,20,13,0,13,20,0,0,20,0,0,20,13,0,13,20,0,0,20,13,0,20,20,0,13,20,0,0,20,13,0,13,20,0,0,20,0,0,20,13,0,13,20,0,0,20,13,0,20,20,0,13"       
      expect_palette("0,40:csh", 0, palette_size, expected_colors) 
        
    if test("it sweeps hues with a custom starting angle"):                                                                                                                  
      expected_colors = "20,20,0,13,20,0,6,20,0,0,20,0,0,20,6,0,20,13,0,20,20,0,13,20,0,6,20,0,0,20,6,0,20,13,0,20,20,0,20,20,0,13,20,0,6,20,0,0,20,6,0,20,13,0"                                                                                                                                             
      expect_palette("60:csh", 0, palette_size, expected_colors)                                                                                                       
                                                                                                                                                            
    if test("it sweeps hues with a custom lightness"):                                                                                                                  
      expected_colors = "10,0,0,10,3,0,10,6,0,10,10,0,6,10,0,3,10,0,0,10,0,0,10,3,0,10,6,0,10,10,0,6,10,0,3,10,0,0,10,3,0,10,6,0,10,10,0,10,10,0,6,10,0,3"                                                                                                                                             
      expect_palette("0,0,10:csh", 0, palette_size, expected_colors)                                                                                                       
                                                                                                                                                            

    if test("it sweeps the right default saturations"):                                                                                                                        
      expected_colors = "20,0,0,20,1,1,20,2,2,20,3,3,20,4,4,20,5,5,20,6,6,20,7,7,20,8,8,20,9,9,20,11,11,20,12,12,20,13,13,20,14,14,20,15,15,20,16,16,20,17,17,20,18,18"                                                                                                                                                    
      expect_palette("css", 0, palette_size, expected_colors)                                                                                                         
                                                                                                                                                              
    if test("it sweeps saturations with a custom hue"):                                                                                                                  
      expected_colors = "0,20,0,1,20,1,2,20,2,3,20,3,4,20,4,5,20,5,6,20,6,7,20,7,8,20,8,9,20,9,11,20,11,12,20,12,13,20,13,14,20,14,15,20,15,16,20,16,17,20,17,18,20,18"
      expect_palette("120:css", 0, palette_size, expected_colors)                                                                                                          
                                                                                                                                          
    if test("it sweeps saturations with a custom step"):                                                                                                                  
      expected_colors = "20,0,0,20,2,2,20,4,4,20,6,6,20,8,8,20,10,10,20,13,13,20,15,15,20,17,17,20,19,19,20,1,1,20,4,4,20,6,6,20,8,8,20,10,10,20,12,12,20,15,15,20,17,17"
      expect_palette("0,28:css", 0, palette_size, expected_colors)                                                                                                          
                                                                                                                                          
    if test("it sweeps saturations with a custom lightness"):                                                                                                                  
      expected_colors = "10,0,0,10,0,0,10,1,1,10,1,1,10,2,2,10,2,2,10,3,3,10,3,3,10,4,4,10,4,4,10,5,5,10,6,6,10,6,6,10,7,7,10,7,7,10,8,8,10,8,8,10,9,9"
      expect_palette("0,0,10:css", 0, palette_size, expected_colors)                                                                                                          
                                                                                                                                          
  
    if test("it sweeps the right default lightnesses"):                                                                                                                        
      expected_colors = "1,0,0,2,0,0,3,0,0,4,0,0,5,0,0,6,0,0,7,0,0,8,0,0,9,0,0,11,0,0,12,0,0,13,0,0,14,0,0,15,0,0,16,0,0,17,0,0,18,0,0,19,0,0"                                                                                                                                                    
      expect_palette("csl", 0, palette_size, expected_colors)                                                                                                         
                                                                                                                                                              
    if test("it sweeps lightnesses with a custom hue"):                                                                                                                  
      expected_colors = "0,0,1,0,0,2,0,0,3,0,0,4,0,0,5,0,0,6,0,0,7,0,0,8,0,0,9,0,0,11,0,0,12,0,0,13,0,0,14,0,0,15,0,0,16,0,0,17,0,0,18,0,0,19"                         
      expect_palette("240:csl", 0, palette_size, expected_colors)                                                                                                          

    if test("it sweeps lightnesses with a custom step"):                                                                                         
      expected_colors = "2,0,0,4,0,0,6,0,0,8,0,0,10,0,0,13,0,0,15,0,0,17,0,0,19,0,0,1,0,0,4,0,0,6,0,0,8,0,0,10,0,0,12,0,0,15,0,0,17,0,0,19,0,0"                                                                                                                                             
      expect_palette("0,28:csl", 0, palette_size, expected_colors)                                                                                                          
                                                                                                                                                                   
    if test("it sweeps lightnesses with a custom lightness scale factor"):                                                                                         
      expected_colors = "0,0,0,1,0,0,1,0,0,2,0,0,2,0,0,3,0,0,3,0,0,4,0,0,4,0,0,5,0,0,6,0,0,6,0,0,7,0,0,7,0,0,8,0,0,8,0,0,9,0,0,9,0,0"                                                                                                                                             
      expect_palette("0,0,10:csl", 0, palette_size, expected_colors)                                                                                                          
                                                                                                                                                                   
    # test that all three args are 0 after the test runs, saw arg2 being 1 as green success leds were pushed out 


########################################################################
# Store & Recall
########################################################################
  if group("storing and recalling arguments"):
    # arg0 = 0                             : (no argument supplied)       restore all arguments from the accumulators
    # arg0 != 0 and arg1  = 0              : (only one argument supplied) arg0=acc0    arg1=arg0    arg2=acc1
    # arg0 != 0 and arg1 != 0              : (two arguments supplied) 
    #                                      :   arg1 shifts -> arg2, arg0 shifts -> arg1 
    #                                      :   arg0 gets set based on arg2
    #                                      :   arg2 = 0 - arg0 = accumulator0
    #                                      :   arg2 = 1 - arg1 = accumulator1
    #                                      :   arg2 = 2 - arg2 = accumulator2

    if test("can store and recall arguments"):
      expect_accumulators("1,2,3:sto:4,5,6:0:rcl:sto", "1,2,3")

    if test("can store and recall empty arguments"):
      expect_accumulators("0:sto:4,5,6:0:rcl:sto", "0,0,0")

    if test("when one argument is supplied, it is shifted to arg1, arg0 is filled from accumulator 0, and arg2 is filled from accumulator1"):
      expect_accumulators("1,2,3:sto:4:rcl:sto", "1,4,2")

    if test("when two/three arguments are supplied, they are shifted to arg1 and arg2, and arg0 is set from an accumulator based on the third argument"):
      expect_accumulators("1,2,3:sto:4,5,0:rcl:sto", "1,4,5") 
      expect_accumulators("1,2,3:sto:4,5,1:rcl:sto", "2,4,5")                                                  
      expect_accumulators("1,2,3:sto:4,5,2:rcl:sto", "3,4,5")

    if test("can successfully shift arguments with sto/rcl tricks"):

# needs a certain brightnss to work

      # no shift 
      expect_render("0:sto:50:rgb", 0, 1, "66,0,0")

      # shift arg0 to arg1, fill arg0 with 0
      expect_render("0:sto:50:rcl:rgb", 0, 1, "0,66,0")

      # shift arg0 to arg2, fill arg0 and arg1 with 0
      expect_render("1,1:sto:50:rcl:sto:1:rcl:rgb", 0, 1, "0,0,66")

      # shift arg0 + arg0 to arg0, arg1
      expect_render("50:sto:50:rcl:rgb", 0, 1, "66,66,0")

      # shft arg0 + arg0 to arg0, arg2, fill arg1 with 0
      expect_render("50:sto:50:rcl:sto:1:rcl:rgb", 0, 1, "66,0,66")

      # shift arg0 + arg0 to arg1, arg2, fill arg0 with 0
      expect_render("1:sto:50:rcl:sto:50:rcl:rgb", 0, 1, "0,66,66")

      # shift arg0 to arg1, fill arg0 with a value
      expect_render("25:sto:50:rcl:rgb", 0, 1, "33,66,0")

      # shift arg0 to arg2, fill arg0 and arg1 with values
      expect_render("25:sto:50:rcl:sto:100:rcl:rgb", 0, 1, "33,132,66")

      # shft arg0 + arg0 to arg0, arg2, fill arg1 with a value
      expect_render("50:sto:50:rcl:sto:25:rcl:rgb", 0, 1, "66,33,66")

      # shift arg0 + arg0 to arg1, arg2, fill arg0 with a value
      expect_render("25:sto:50:rcl:sto:50:rcl:rgb", 0, 1, "33,66,66")

      test("stores by copying with fill of two arguments supplied")
      expect_accumulators("1,2,3:sto:4,5:sto", "4,5,0")

      test("stores by copying if three arguments supplied")
      expect_accumulators("1,2,3:sto:4,5,6:sto", "4,5,6")

      test("stores zeros in all accumulators if arg0 is 0")
      expect_accumulators("1,2,3:sto:0:sto", "0,0,0")

      test("pushes argument to accumulators")
      expect_accumulators("1,2,3:sto:4:psh", "4,1,2")

      test("can push arguments multiple times to accumulators")
      expect_accumulators("1,2,3:sto:4:psh:5:psh:6:psh", "6,5,4")

      test("can push multiple arguments to accumulators")
      expect_accumulators("1,2,3:sto:4,5:psh", "4,5,1")


########################################################################
# Configuring
########################################################################
  if group("setting configuration values"):

    if test("the fade rate can be reset to the default"):
      expect_int("2,1000:cfg:0,8:tst", 1000)
      default = lc.command_int("0,7:tst")
      expect_int("2,0:cfg:0,8:tst", default)

    if test("the effect can be set to a default value"):
      lc.command_str("3,20:cfg")
      expect_effect("red:flu", 0, 1, "20")


########################################################################
# Dynamic Color Placement
########################################################################
  if group("setting dynamic colors"):

    if test("a palette color can be placed dynamically"):
      expect_buffer("5:dyn", 0, 1, "5,0,0")

    if test("a different palette color can be placed dynamically"):
      expect_buffer("11:dyn", 0, 1, "11,0,0")

    if test("a primary and a secondary palette color can be placed dynamically"):
      expect_buffer("3,12:dyn", 0, 1, "3,12,0")

    if test("colors and a rendering hint can be placed"):
      # there are current no hints, so any value is ignored
      expect_buffer("4,9,1:dyn", 0, 1, "4,9,1")


########################################################################
# Dynamic Color Rendering
########################################################################
  if group("dynamic color rendering"):

    if test("a dynamically placed color can be rendered"):
      expect_render("2:dyn", 0, 1, "51,51,0")

    if test("a different dynamically placed color can be rendered"):
      expect_render("10:dyn", 0, 1, "0,51,25")

    if test("shuffling the palette causes the rendered colors to change"):
      expect_render("0:dyn:1:dyn:2:dyn:3:dyn:4:dyn:5:dyn", 0, 6, "25,0,51,0,0,51,0,51,0,51,51,0,51,25,0,51,0,0")    
      expect_render("shf:0:dyn:1:dyn:2:dyn:3:dyn:4:dyn:5:dyn", 0, 6,"51,51,0,0,25,51,0,51,0,0,0,51,38,51,0,51,25,0")


########################################################################
# Dynamic Color Effects
########################################################################
  if group("setting dynamic color effects"):

    if test("dynamic blink"):

      # set blink period to minimum value
      lc.command_str("0,6:cfg")

      # set a macro to advance the blink period
      lc.command_str("0:set:6:tst:flu")

      # place alternating dynamic colors
      lc.command_str("0,4:dyn:bld")

      # simulate a half blink period
      expect_render("0,3:run", 0, 1, "0,0,51", False)

      # simulate a full blink period
      expect_render("0,6:run", 0, 1, "51,0,0", False)


########################################################################
# Dynamic Color Handling
########################################################################
  if group("setting dynamic color effects"):

    if test("should be able to copy dynamic colors"):
      expect_buffer("12:dyn:1,2:cpy", 0, 2, "12,0,0,12,0,0")

    if test("should be able to copy dynamic color marker"):
      expect_effect("12:dyn:1,2:cpy", 0, 2, "128,128")

    if test("should be able to copy dynamic colors with zooming"):
      expect_buffer("12:dyn:1,2,2:cpy", 0, 4, "12,0,0,12,0,0,12,0,0,12,0,0")
  
    if test("should be able to copy dynamic color marker with zooming"):
      expect_effect("12:dyn:1,2,2:cpy", 0, 4, "128,128,128,128")

    if test("copied dynamic color should render properly"):
      expect_render("8:dyn:1,2:cpy", 0, 2, "0,25,51,0,25,51")

    if test("copied dynamic color with zooming should render properly"):
      expect_render("12:dyn:1,2,2:cpy", 0, 4, "51,38,0,51,38,0,51,38,0,51,38,0")


########################################################################
# Math operations
########################################################################
  if group("math operations"):

    if test("should be able to add two values"):
      expect_accumulators("2,5:psh:add", "7,0,0")

    if test("should be able to add positibve and negative values"):
      expect_accumulators("3,-3:psh:add", "0,0,0")

    if test("should be able to add positive and negative values and get a negative result"):
      expect_accumulators("0,-3:psh:add", "-3,0,0")

    if test("should not crash when adding two large integers"):
      expect_accumulators("32767,32767:psh:add", "-2,0,0")

    if test("should not crash when adding two crazy large numbers"):
      expect_accumulators("99999,99999:psh:add", "3390,0,0")


    if test("should be able to subtract two values"):
      expect_accumulators("7,5:psh:sub", "2,0,0")

    if test("should be able to subtract negative and positive values"):
      expect_accumulators("0,-3:psh:sub", "3,0,0")

    if test("should not crash when subtracting two large integers"):
      expect_accumulators("32767,32767:psh:sub", "0,0,0")

    if test("should not crash when subtracting two crazy large numbers"):
      expect_accumulators("99999,99999:psh:sub", "0,0,0")
    

    if test("should be able to multiply two values"):
      expect_accumulators("2,5:psh:mul", "10,0,0")

    if test("should be able to multiply by zero"):
      expect_accumulators("0,1000:psh:mul", "0,0,0")

    if test("should be able to multiply by a negative number"):
      expect_accumulators("2,-1:psh:mul", "-2,0,0")

    if test("should not crash when multiplying two large integers"):
      expect_accumulators("32767,32767:psh:mul", "1,0,0")

    if test("should not crash when multiplying two crazy large numbers"):
      expect_accumulators("99999,99999:psh:mul", "-10559,0,0")


    if test("should be able to divide two values"):
      expect_accumulators("3,10:psh:div", "3,0,0")

    if test("should be able to divide zero by something"):
      expect_accumulators("5,0:psh:div", "0,0,0")

    if test("a divide by zero is ignored, leaving accumulators as were"):
      expect_accumulators("0,5:psh:div", "0,5,0")

    if test("should be able to divide a negative number"):
      expect_accumulators("2,-5:psh:div", "-2,0,0")

    if test("should be able to divide by a negative number"):
      expect_accumulators("-2, 5:psh:div", "-2,0,0")

    if test("should not crash when dividing two large integers"):
      expect_accumulators("32767,32767:psh:div", "1,0,0")

    if test("should not crash when dividing two crazy large numbers"):
      expect_accumulators("99999,99999:psh:div", "1,0,0")


    if test("should be able to modulus two values"):
      expect_accumulators("3,10:psh:mod", "1,0,0")

    if test("should be able to modulus zero by something"):
      expect_accumulators("3,0,1:psh:mod", "0,0,0")

    if test("a modulus by zero is ignored, leaving accumulators as were"):
      expect_accumulators("0,5:psh:mod", "0,5,0")

    if test("should be able to modulus a negative nunmber"):
      # mod returns an absolute value to ease restraining pixel location
      expect_accumulators("3,-10:psh:mod", "1,0,0")

    if test("should be able to modulus by a negative nunmber"):
      expect_accumulators("-3,10:psh:mod", "1,0,0")

    if test("should not crash when modulusing two large integers"):
      expect_accumulators("32767,32767:psh:mod", "0,0,0")

    if test("should not crash when modulusing two crazy large numbers"):
      expect_accumulators("99999,99999:psh:mod", "0,0,0")

    if test("mod operation should also return a positive value"):
      # mod returns an absolute value to ease restraining pixel location
      expect_accumulators("90,-26:psh:mod", "26,0,0")


    if test("should be able to diff two values"):
      expect_accumulators("27,45:psh:dif", "18,0,0")

    if test("should be able to diff two values in either direction"):
      expect_accumulators("45,27:psh:dif", "18,0,0")

    if test("should be able to diff two negative values"):
      expect_accumulators("-27,-45:psh:dif", "18,0,0")

    if test("should be able to diff two negative values in either direction"):
      expect_accumulators("-45,-27:psh:dif", "18,0,0")


    if test("should be able to average two values"):
      expect_accumulators("27,45:psh:avg", "36,0,0")

    if test("should be able to average two values in either direction"):
      expect_accumulators("27,45:psh:avg", "36,0,0")

    if test("should be able to average two negative values"):
      expect_accumulators("-27,-45:psh:avg", "-36,0,0")

    if test("should be able to average two negative values in either direction"):
      expect_accumulators("-45,-27:psh:avg", "-36,0,0")

    if test("mth operations should automatically recall accumulators into arguments"):
      expect_accumulators("3,9:psh:mul:sto", "27,0,0")



########################################################################                     
########################################################################                     
 
def run():                                  
  print
  specs()
  print 

  for error in test_failures:
    print error,
  print
  
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

  pre_test_reset()
  if skip_led_report == True:
    lc.command_str("stp")
  else:
    total = success_count + failure_count + num_pending + num_skipped
    if total > 0:
      show_success = int(0.5 + (success_count * num_leds / total))
      show_failure = int(0.5 + ((failure_count + num_skipped) * num_leds / total))
      show_pending = int(0.5 + (num_pending * num_leds / total))

      print "show success: " + str(show_success)
      print "show failure: " + str(show_failure)
      print "show pending: " + str(show_pending)

      lc.command("stp:0:lev:0,0:cfg:1,0:cfg:2,0:cfg")

      if show_success + show_failure + show_pending > num_leds:
        print "show success: " + str(show_success)
        print "show failure: " + str(show_failure)
        print "show pending: " + str(show_pending)
      else:
        if show_success > 0:
          lc.command(str(show_success) + ":grn") 
        if show_failure > 0:  
          lc.command(str(show_failure) + ":red")                                                                                                                                                                  
        if show_success > 0:
          lc.command(str(show_pending) + ":yel")                                                                                                                                                                  

      lc.command("flu:cnt")

if __name__ == '__main__': 
  initialize() 
  run()
  print


#def print_frame():
#  callerframerecord = inspect.stack()[1]    # 0 represents this line
#                                            # 1 represents line at caller
#  frame = callerframerecord[0]
#  info = inspect.getframeinfo(frame)
#  print info.filename                       # __FILE__     -> Test.py
#  print info.function                       # __FUNCTION__ -> Main
#  print info.lineno                         # __LINE__     -> 13


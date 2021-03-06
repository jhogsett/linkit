#!/usr/bin/python

import sys
import macro_compiler as mc
import os
import app_ui as ui
import terminal_colors as tc
import argparse
import led_command as lc
import math
import utils as u
import datetime

global app_description, verbose_mode, random_seed
app_description = None 
verbose_mode = None
random_seed = 1

global device_profile, num_leds, starting_macro, num_macro_chars, ending_macro, number_of_macros, char_buffer_size, number_of_fine_zones, number_of_colors, number_of_sequencers, quiet_mode
device_profile = None
num_leds = None
starting_macro = None
num_macro_chars = None
ending_macro = None
number_of_macros = None
char_buffer_size = None
number_of_fine_zones = None
number_of_colors = None
number_of_sequencers = None
quiet_mode = None

def get_options():
  global verbose_mode, quiet_mode
  parser = argparse.ArgumentParser(description=app_description)
  parser.add_argument("-v", "--verbose", dest="verbose", action="store_true", help="display verbose info (False)")
  parser.add_argument("-q", "--quiet", dest="quiet", action="store_true", help="don't use terminal colors (False)")
  args = parser.parse_args()
  verbose_mode = args.verbose
  quiet_mode = args.quiet

def initialize():
    global app_description
    global device_profile, num_leds, starting_macro, num_macro_chars, ending_macro, number_of_macros, char_buffer_size, number_of_fine_zones, number_of_colors, number_of_sequencers
    app_description = "Apollo Lighting System - Macro Compiler Specs v.2.0 12-0-2018"
    get_options()
    ui.begin(verbose_mode, quiet_mode)
    lc.begin(verbose_mode)
    lc.stop_all()

    device_profile = lc.get_device_profile()
    num_leds = device_profile["NUM-LEDS"]
    starting_macro = device_profile["START-MACRO"]
    num_macro_chars = device_profile["NUM-MACRO-BYTES"]
    ending_macro = device_profile["END-MACRO"]
    number_of_macros = device_profile["NUM-MACROS"]
    char_buffer_size = device_profile["CHAR-BUFFER-SIZE"]
    number_of_fine_zones = device_profile["NUM-FINE-ZONES"]
    number_of_colors = device_profile["NUM-PALETTE-COLORS"]
    number_of_sequencers = device_profile["NUM-SEQUENCERS"]
    total_macro_bytes = device_profile["TOTAL-MACRO-BYTES"]
    last_macro_bytes = device_profile["LAST-MACRO-BYTES"]

    mc.begin(lc, verbose_mode, quiet_mode, presets(), starting_macro, ending_macro, number_of_sequencers, num_macro_chars, char_buffer_size, last_macro_bytes)

    ui.app_description(app_description)
    ui.report_info(ui.intro_entry("Number of LEDs", num_leds))
    ui.report_info(ui.intro_entry("Number of macros", number_of_macros))
    ui.report_info(ui.intro_entry("Number of sequencers", number_of_sequencers))
    ui.report_info(ui.intro_entry("Bytes per macro", num_macro_chars))
    ui.report_info(ui.intro_entry("First macro", starting_macro))
    ui.report_info(ui.intro_entry("Last macro", ending_macro))
    ui.report_info(ui.intro_entry("Last macro bytes", last_macro_bytes))
    ui.report_info(ui.intro_entry("Char buffer size", char_buffer_size))

def presets():
    return {
      "NUM-LEDS": num_leds,
      "NUM-MACROS": number_of_macros,
      "NUM-SEQUENCERS": number_of_sequencers,
      "START-MACRO": starting_macro,
      "END-MACRO": ending_macro,
      "NUM-MACRO-CHARS": num_macro_chars,
      "CHAR-BUFFER-SIZE": char_buffer_size,
      "NUM-SEQUENCERS": number_of_sequencers,
      "NUM-FINE-ZONES": number_of_fine_zones
    }

def report_failed(description, expected, got):
  description_ = tc.yellow(description)
  expected_ = "\t" + tc.cyan("Expected:") + "\n\t\t" + tc.green(str(expected))      
  got_ = "\t" + tc.cyan("Got:     ") + "\n\t\t" + tc.red(str(got))
  ui.report_error("\nTest failed! %s\n%s\n%s\n" % (description_, expected_, got_))

def report_failed_script(description, expected, got):
  description_ = tc.yellow(description)
  expected_ = tc.cyan("Expected:\n")
  for line in expected:
    expected_ += tc.green(line) + "\n"
  got_ = tc.cyan("Got:\n")
  for line in got:
    got_ += tc.red(line) + "\n"
  ui.report_error("\nTest failed! %s\n%s\n%s\n" % (description_, expected_, got_))

def report_worked(description, expected, got):
  description_ = tc.yellow(description)
  expected_ = "\t" + tc.cyan("Expected:") + "\n\t\t" + tc.green(str(expected))
  got_ = "\t" + tc.cyan("Got Same:") + "\n\t\t" + tc.red(str(got))
  ui.report_error("\nTest failed! %s\n%s\n%s\n" % (description_, expected_, got_))

def report_worked_script(description, expected, got):
  description_ = tc.yellow(description)
  expected_ = tc.cyan("Expected:\n")
  for line in expected:
    expected_ += tc.green(line) + "\n"
  got_ = tc.cyan("Got Same:\n")
  for line in got:
    got_ += tc.red(line) + "\n"
  ui.report_error("\nTest failed! %s\n%s\n%s\n" % (description_, expected_, got_))

def report_success(description, expected, got):
  if not verbose_mode:
    sys.stdout.write(tc.green("."))
    sys.stdout.flush()
  
def report_test(type, description):
  print tc.cyan(type) + " " + tc.green(description)

def expect(description, got, expected):
  if expected != got:
    report_failed(description, expected, got)
  else:
    report_success(description, expected, got)

def expect_script(description, got, expected):
  if expected != got:
    report_failed_script(description, expected, got)
  else:
    report_success(description, expected, got)

def not_expect(description, got, expected):
  if expected == got:
    report_worked(description, expected, got)
  else:
    report_success(description, expected, got)

def not_expect_script(description, got, expected):
  if expected == got:
    report_worked_script(description, expected, got)
  else:
    report_success(description, expected, got)

def print_script(script):
  for line in script:
    print line
  print

def test(description):
  return True

########################################################################
########################################################################

def do_compilation(filename):
    u.randomize(random_seed)
    return mc.compile_file(filename)

def specs():

  if verbose_mode:
    report_test("String manipulation tests", "extract_args()")
  expect("extract args 1", u.extract_args("[test]", {"[" : "]"}), ["test"])
  expect("extract args 2", u.extract_args(" [test] ", {"[" : "]"}), ["test"])
  expect("extract args 3", u.extract_args("[ test ]", {"[" : "]"}), ["test"])
  expect("extract args 4", u.extract_args("/test/", {"/" : "/"}), ["test"])
  expect("extract args 5", u.extract_args("(t e s t)", {"(" : ")"}), ["t", "e", "s", "t"])
  expect("extract args 6", u.extract_args("[test] abc", {"[" : "]"}), ["test"])
  expect("extract args 7", u.extract_args("abc [test] def", {"[" : "]"}), ["test"])
  expect("extract args 8", u.extract_args("(t  e  s  t)", {"(" : ")"}), ["t", "e", "s", "t"])
  expect("extract args 9", u.extract_args("abc [test] def [test2]", {"[" : "]"}), ["test"])
  expect("extract args 10", u.extract_args("( t e s t )", {"(" : ")"}), ["t", "e", "s", "t"])
  expect("extract args 11", u.extract_args("[test", {"[" : "]"}), [])
  expect("extract args 12", u.extract_args("test]", {"[" : "]"}), [])
  expect("extract args 13", u.extract_args("test", {"[" : "]"}), [])
  expect("extract args 14", u.extract_args("[test", {"[" : "]"}), [])
  expect("extract args 15", u.extract_args("[]", {"[" : "]"}), [])
  expect("extract args 16", u.extract_args("[[test]]", {"[[" : "]]"}), ["test"])
  expect("extract args 16", u.extract_args("[[[test]]]", {"[[[" : "]]]"}), ["test"])
  expect("extract args 17", u.extract_args("(((test 1 2 3)))", {"(((" : ")))"}), ["test", '1', '2', '3'])

  if verbose_mode:
    report_test("String manipulation tests", "replace_args()")
  expect("replace args 1", u.replace_args("[test]", {"[" : "]"}, "abc"), "abc")
  expect("replace args 2", u.replace_args(" [test] ", {"[" : "]"}, "abc"), " abc ")
  expect("replace args 3", u.replace_args("[test][]", {"[" : "]"}, "abc"), "abc[]")
  expect("replace args 4", u.replace_args("[test", {"[" : "]"}, "abc"), "[test")
  expect("replace args 5", u.replace_args("[]", {"[" : "]"}, "abc"), "abc")

  if verbose_mode:
    report_test("String manipulation tests", "get_key_args()")
  expect("get key args 1", u.get_key_args("$abc", "$"), ["abc"])
  expect("get key args 2", u.get_key_args(" $abc", "$"), ["abc"])
  expect("get key args 3", u.get_key_args("$abc ", "$"), ["abc"])
  expect("get key args 4", u.get_key_args(" $abc ", "$"), ["abc"])
  expect("get key args 5", u.get_key_args("$abc def", "$"), ["abc", "def"])
  expect("get key args 6", u.get_key_args("$abc  def", "$"), ["abc", "def"])
  expect("get key args 7", u.get_key_args("$", "$"), [])
  expect("get key args 8", u.get_key_args("", "$"), [])
  expect("get key args 1", u.get_key_args("$$abc", "$$"), ["abc"])

  # crash tests
  # these are expected to raise compilation errors
  fixture_filename = "spec_fixtures/crash_script%d.mac"
  expected_filename = "spec_fixtures/crash_script%d_expected.txt"
  script_number = 1
  while(True):
    fixture_file = fixture_filename % script_number
    expected_file = expected_filename % script_number
    script_number += 1
    if(os.path.exists(fixture_file)):
      if verbose_mode:
        report_test("Crash script", fixture_file)
      try:
        compiled_script = do_compilation(fixture_file)
        expect("Script " + fixture_file + " was expected to raise an error", "no error raised", "error raised")
        if verbose_mode:
          print_script(compiled_script)
      except ValueError as error:
        expected_error = mc.load_file(expected_file, ".txt")
        expect("Compilation crashes with expected message - script: " + fixture_file, [str(error)], expected_error)
        continue
      finally:
        mc.reset()
    else:
      break

  # negative tests
  # these are expected to fail to compile but not crash
  fixture_filename = "spec_fixtures/bad_script%d.mac"
  script_number = 1
  while(True):
    fixture_file = fixture_filename % script_number
    script_number += 1
    if(os.path.exists(fixture_file)):
      if verbose_mode:
        report_test("Negative script", fixture_file)
      try:
        compiled_script = do_compilation(fixture_file)
        if verbose_mode:
          print_script(compiled_script)
        expect("Invalid compilation of: " + fixture_file, mc.compilation_valid(compiled_script), False)
        mc.reset()
      except ValueError, e:
        ui.report_error("Compilation error in " + fixture_file + ": " + str(e))
        print_script(mc.get_saved_bad_script())
        expect("Exception caught", True, False)
    else:
      break

  # positive tests
  fixture_filename = "spec_fixtures/test_script%d.mac"
  expected_filename = "spec_fixtures/test_script%d_expected.txt"
  script_number = 1
  while(True):
    fixture_file = fixture_filename % script_number
    expected_file = expected_filename % script_number
    script_number += 1
    if(os.path.exists(fixture_file)):
      if verbose_mode:
        report_test("Positive script", fixture_file)
      try:
        compiled_script = do_compilation(fixture_file)
      except ValueError, e:
        ui.report_error("Compilation error in " + fixture_file + ": " + str(e))
        break
      if verbose_mode:
        print_script(compiled_script)
      expected_script = mc.load_file(expected_file, ".txt")
      expect_script("Valid compilation of: " + fixture_file, compiled_script, expected_script)
      mc.reset()
    else:
      break


############################################################################

def setup():
  initialize()

def loop():
  start_time = datetime.datetime.now()
  specs() 
  end_time = datetime.datetime.now()

  print

  print tc.white("tests ran in: " + str(end_time - start_time))
  print

  sys.exit()

if __name__ == '__main__':
  setup()
#  while True:
#    try:
  loop()
#    except KeyboardInterrupt:
#      sys.exit("\nExiting...\n")
#    except Exception:
#      raise


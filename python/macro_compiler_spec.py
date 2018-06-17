#!/usr/bin/python

import sys
import macro_compiler as mc
import os
import app_ui as ui
import terminal_colors as tc
import argparse
import led_command as lc
import math

global app_description, verbose_mode, num_leds, starting_macro, num_macro_chars, ending_macro, char_buffer_size, number_of_sequencers, number_of_macros
app_description = None
verbose_mode = None
num_leds = None
starting_macro = None
num_macro_chars = None
ending_macro = None
char_buffer_size = None
number_of_sequencers = None
number_of_macros = None

def get_options():
  global verbose_mode
  parser = argparse.ArgumentParser(description=app_description)
  parser.add_argument("-v", "--verbose", dest="verbose", action="store_true", help="display verbose info (False)")
  args = parser.parse_args()
  verbose_mode = args.verbose

def initialize():
    global app_description, num_leds, starting_macro, num_macro_chars, ending_macro, char_buffer_size, number_of_sequencers, number_of_macros
    app_description = "Apollo Lighting System - Macro Compiler Specs v.0.0 6-0-2018"
    get_options()

    ui.begin(verbose_mode)

    lc.begin(verbose_mode)
    lc.attention()
    lc.stop_all()
    num_leds = lc.get_num_leds()
    starting_macro = lc.get_first_eeprom_macro()
    num_macro_chars = lc.get_num_macro_chars()
    ending_macro = starting_macro + int(math.ceil(1024.0 / num_macro_chars) - 1)
    char_buffer_size = lc.get_max_string_length()
    number_of_sequencers = lc.get_num_sequencers()
    number_of_macros = (ending_macro - starting_macro) + 1
    mc.begin(lc, verbose_mode, presets(), starting_macro, ending_macro, number_of_sequencers, num_macro_chars, char_buffer_size)
    ui.report_info(ui.intro_entry("Number of LEDs", num_leds))
    ui.report_info(ui.intro_entry("Number of macros", (ending_macro - starting_macro) + 1))
    ui.report_info(ui.intro_entry("Number of sequencers", number_of_sequencers))
    ui.report_info(ui.intro_entry("Bytes per macro", num_macro_chars))
    ui.report_info(ui.intro_entry("First macro", starting_macro))
    ui.report_info(ui.intro_entry("Last macro", ending_macro))
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
      "NUM-SEQUENCERS": number_of_sequencers
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

def specs():

  if verbose_mode:
    report_test("String manipulation tests", "extract_args()")
  expect("extract args 1", mc.extract_args("[test]", "[", "]"), ["test"])
  expect("extract args 2", mc.extract_args(" [test] ", "[", "]"), ["test"])
  expect("extract args 3", mc.extract_args("[ test ]", "[", "]"), ["test"])
  expect("extract args 4", mc.extract_args("/test/", "/", "/"), ["test"])
  expect("extract args 5", mc.extract_args("(t e s t)", "(", ")"), ["t", "e", "s", "t"])
  expect("extract args 6", mc.extract_args("[test] abc", "[", "]"), ["test"])
  expect("extract args 7", mc.extract_args("abc [test] def", "[", "]"), ["test"])
  expect("extract args 8", mc.extract_args("(t  e  s  t)", "(", ")"), ["t", "e", "s", "t"])
  expect("extract args 9", mc.extract_args("abc [test] def [test2]", "[", "]"), ["test"])
  expect("extract args 10", mc.extract_args("( t e s t )", "(", ")"), ["t", "e", "s", "t"])
  expect("extract args 11", mc.extract_args("[test", "[", "]"), [])
  expect("extract args 12", mc.extract_args("test]", "[", "]"), [])
  expect("extract args 13", mc.extract_args("test", "[", "]"), [])
  expect("extract args 14", mc.extract_args("[test", "[", "]"), [])
  expect("extract args 15", mc.extract_args("[]", "[", "]"), [])
  expect("extract args 16", mc.extract_args("[[test]]", "[[", "]]"), ["test"])
  expect("extract args 16", mc.extract_args("[[[test]]]", "[[[", "]]]"), ["test"])
  expect("extract args 17", mc.extract_args("(((test 1 2 3)))", "(((", ")))"), ["test", '1', '2', '3'])

  if verbose_mode:
    report_test("String manipulation tests", "replace_args()")
  expect("replace args 1", mc.replace_args("[test]", "[", "]", "abc"), "abc")
  expect("replace args 2", mc.replace_args(" [test] ", "[", "]", "abc"), " abc ")
  expect("replace args 3", mc.replace_args("[test][]", "[", "]", "abc"), "abc[]")
  expect("replace args 4", mc.replace_args("[test", "[", "]", "abc"), "[test")
  expect("replace args 5", mc.replace_args("[]", "[", "]", "abc"), "abc")

  if verbose_mode:
    report_test("String manipulation tests", "get_key_args()")
  expect("get key args 1", mc.get_key_args("$abc", "$"), ["abc"])
  expect("get key args 2", mc.get_key_args(" $abc", "$"), ["abc"])
  expect("get key args 3", mc.get_key_args("$abc ", "$"), ["abc"])
  expect("get key args 4", mc.get_key_args(" $abc ", "$"), ["abc"])
  expect("get key args 5", mc.get_key_args("$abc def", "$"), ["abc", "def"])
  expect("get key args 6", mc.get_key_args("$abc  def", "$"), ["abc", "def"])
  expect("get key args 7", mc.get_key_args("$", "$"), [])
  expect("get key args 8", mc.get_key_args("", "$"), [])
  expect("get key args 1", mc.get_key_args("$$abc", "$$"), ["abc"])

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
        compiled_script = mc.compile_file(fixture_file)
      except ValueError, e:
        ui.report_error("Compilation error: " + str(e))
        break
      if verbose_mode:
        print_script(compiled_script)
      expected_script = mc.load_file(expected_file, ".txt")
      expect_script("Valid compilation of: " + fixture_file, compiled_script, expected_script)
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
        compiled_script = mc.compile_file(fixture_file)
        if verbose_mode:
          print_script(compiled_script)
        expect("Invalid compilation of: " + fixture_file, mc.compilation_valid(compiled_script), False)
        mc.reset()
      except ValueError, e:
        ui.report_error("Compilation error: " + str(e))
        print_script(mc.get_saved_bad_script())
        expect("Exception caught", True, False)
    else:
      break

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
        compiled_script = mc.compile_file(fixture_file)
        expect("Script raised an error", True, False)
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

############################################################################

def setup():
  initialize()

def loop():
  specs() 
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


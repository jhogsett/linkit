#!/usr/bin/python

import sys
import macro_compiler as mc
import os

def initialize():
  mc.begin(False)

def expect(description, expected, got):
  if expected != got:
    print "failed: " + description
    print "expected: " + str(expected)
    print "got: " + str(got)

def not_expect(description, expected, got):
  if expected == got:
    print "failed: " + description
    print "expected: " + str(expected)
    print "got (matches): " + str(got)

def print_script(script):
  for line in script:
    print line
  print

def test(description):
  return True

########################################################################
########################################################################

def specs():

  # positive tests
  fixture_filename = "spec_fixtures/test_script%d.mac"
  expected_filename = "spec_fixtures/test_script%d_expected.txt"
  script_number = 1
  while(True):
    fixture_file = fixture_filename % script_number
    expected_file = expected_filename % script_number
    script_number += 1
    if(os.path.exists(fixture_file)):
      print "Testing file (pos): " + fixture_file
      compiled_script = mc.compile_file(fixture_file)
      print_script(compiled_script)
      expected_script = mc.load_file(expected_file, ".txt")
      expect("compiled script", expected_script, compiled_script)
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
      print "Testing file (neg): " + fixture_file
      compiled_script = mc.compile_file(fixture_file)
      print_script(compiled_script)
      expect("compilation valid", False, mc.compilation_valid(compiled_script))
      mc.reset()
    else:
      break


############################################################################

def setup():
  initialize()
#    introduction()
#  pass

def loop():
  specs() 
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


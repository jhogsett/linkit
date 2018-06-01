#!/usr/bin/python

import sys
import macro_compiler as mc
import os

def initialize():
  mc.begin(False)

def expect(description, got, expected):
  if expected != got:
    print "failed: " + description
    print "expected: " + str(expected)
    print "got: " + str(got)

def not_expect(description, got, expected):
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

  expect("replace args 1", mc.replace_args("[test]", "[", "]", "abc"), "abc")
  expect("replace args 2", mc.replace_args(" [test] ", "[", "]", "abc"), " abc ")
  expect("replace args 3", mc.replace_args("[test][]", "[", "]", "abc"), "abc[]")
  expect("replace args 4", mc.replace_args("[test", "[", "]", "abc"), "[test")
  expect("replace args 5", mc.replace_args("[]", "[", "]", "abc"), "abc")

  expect("get key args 1", mc.get_key_args("$abc", "$"), ["abc"])
  expect("get key args 2", mc.get_key_args(" $abc", "$"), ["abc"])
  expect("get key args 3", mc.get_key_args("$abc ", "$"), ["abc"])
  expect("get key args 4", mc.get_key_args(" $abc ", "$"), ["abc"])
  expect("get key args 5", mc.get_key_args("$abc def", "$"), ["abc", "def"])
  expect("get key args 6", mc.get_key_args("$abc  def", "$"), ["abc", "def"])
  expect("get key args 7", mc.get_key_args("$", "$"), [])
  expect("get key args 8", mc.get_key_args("", "$"), [])

  # positive tests
  fixture_filename = "spec_fixtures/test_script%d.mac"
  expected_filename = "spec_fixtures/test_script%d_expected.txt"
  script_number = 1
  while(True):
    fixture_file = fixture_filename % script_number
    expected_file = expected_filename % script_number
    script_number += 1
    if(os.path.exists(fixture_file)):
      print "-----------------------------------------------"
      print "Testing file (pos): " + fixture_file
      compiled_script = mc.compile_file(fixture_file)
      print_script(compiled_script)
      expected_script = mc.load_file(expected_file, ".txt")
      expect("compiled script", compiled_script, expected_script)
      print
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
      print "-----------------------------------------------"
      print "Testing file (neg): " + fixture_file
      compiled_script = mc.compile_file(fixture_file)
      print_script(compiled_script)
      expect("compilation valid", mc.compilation_valid(compiled_script), False)
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


#!/usr/bin/python

import sys
import macro_compiler as mc

def expect(description, expected, got):
  if expected != got:
    print "failed: " + description
    print "expected: " + str(expected)
    print "got: " + str(got)

def print_script(script):
  for line in script:
    print line
  print

def test(description):
  return True

########################################################################
########################################################################

def specs():

########################################################################
# simple script
########################################################################
  if test("simple script"):
    compiled_script = mc.compile_file("spec_fixtures/test_script1.mac")
    print_script(compiled_script)
    expected_script = mc.load_file("spec_fixtures/test_script1_expected", ".txt")
    expect("compiled script #1", expected_script, compiled_script)
    mc.reset()


########################################################################
# complex script
########################################################################  
  if test("complex script"):
    compiled_script = mc.compile_file("spec_fixtures/test_script2.mac")
    print_script(compiled_script)
    expected_script = mc.load_file("spec_fixtures/test_script2_expected", ".txt")
    expect("compiled script #2", expected_script, compiled_script)
    mc.reset()

  if test("complex script 2"):
    compiled_script = mc.compile_file("spec_fixtures/test_script2.mac")
    print_script(compiled_script)
    expected_script = mc.load_file("spec_fixtures/test_script3_expected", ".txt")
    expect("complex script #2", expected_script, compiled_script)
    mc.reset()

########################################################################
# include script
########################################################################
  if test("include script"):
    compiled_script = mc.compile_file("spec_fixtures/include_test.mac")
    print_script(compiled_script)
    expected_script = mc.load_file("spec_fixtures/include_test_expected", ".txt")
    expect("include script", expected_script, compiled_script)
    mc.reset()


############################################################################

def setup():
#    initialize()
#    introduction()
  pass

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


#!/usr/bin/python

import socket
import struct
import sys
import terminal_colors as tc
import argparse
import serial
import time

app_description = "Python Application Template for Apollo Lighting System v.0.0 10-30-2017"

parser = argparse.ArgumentParser(description=app_description)
parser.add_argument("-v", "--verbose", dest="verbose", action='store_true', help='display verbose info (False)')

args = parser.parse_args()
verbose_mode = args.verbose

server_name = socket.getfqdn()

global s
s = serial.Serial("/dev/ttyS0", 115200)

response_wait = 0.1

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

def set_macro(macro, macro_text, expected_bytes):
  bytes = command_int(str(macro) + ":set:" + macro_text)
  print tc.cyan(str(bytes) + " bytes")
  if verbose_mode:
    print tc.yellow("compiled macro: " + command_str("1," + str(macro) + ":tst"))
  if expected_bytes > 0 and expected_bytes != bytes:
    print tc.red("Wrong number of bytes received for macro #") + tc.white(str(macro)) + tc.red("- retrying")
    set_macro(macro, macro_text, expected_bytes)


def setup():
  global s, debug_mode
  s = serial.Serial("/dev/ttyS0", 115200)
  command_str(":::stp:stp")

  if len(sys.argv) > 3:
    if(sys.argv[3] == "debug"):
      debug_mode = True


def loop():
  print "all done!"
  sys.exit()


if __name__ == '__main__':
  print tc.magenta("\n" + app_description + "\n")
  if verbose_mode:
    print tc.yellow("verbose mode: on")
  print tc.cyan("machine: " + server_name)
  print

  setup()

  while True:
    try:  
      loop()
    except EOFError:
      sys.exit("\nExiting...\n")
    except KeyboardInterrupt:
      sys.exit("\nExiting...\n")
    except Exception:
      raise
    finally:
      pass


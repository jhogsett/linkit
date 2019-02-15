#!/usr/bin/python

import sys
import argparse
import terminal_colors as tc
import app_ui as ui
import os
import utils
import time
#import datetime

global plans, pre_command_line, post_command_line, round_time
plans = []
pre_command_line = None
post_command_line = None
round_time = 0

############################################################

def ingest_file(filename):
  return utils.load_file(filename, ".run")

def preprocess(lines):
  return utils.strip_comments(lines)

def extract_settings(lines):
  global pre_command_line, post_command_line, round_time
  pre_command_line = lines[0]
  post_command_line = lines[1]
  round_time = int(lines[2])
  return lines[3:]

def parse(script):
  plans = []
  for line in script:
    result = parsing_handler1(line)
    if not result:
      result = parsing_handler1(line)
    if not result:
      result = parsing_handler3(line)
    if result:
      plans.append(result)
  return plans

#num-boxes 1-5
# 1) integer range (second arg has a '-' that splits into two valid integers)
def parsing_handler1(line):
  parts = line.split()
  if len(parts) == 2:
    name = parts[0]
    range = parts[1].split("-")
    if len(range) == 2:
      return {"type" : 1, "name" : name, "low" : range[0], "high" : range[1]}
  return None

#refresh-time 40-120 10
# 2) integer range with quantization (+ third arg has an int)
def parsing_handler2(line):
  parts = line.split()
  if len(parts) == 3:
    name = parts[0]
    range = parts[1].split("-")
    quant = parts[2]
    if len(range) == 2:
      return {"type" : 2, "name" : name, "low" : range[0], "high" : range[1], "quant" : quant}
  return None

#ball-sequencer seq sqs sws swc
# 3) series of values if the above two are not met
def parsing_handler3(line):
  parts = line.split()
  if len(parts) > 1:
    name = parts[0]
    choices = parts[1:]
    if len(choices) > 0:
      return {"type" : 3, "name" : name, "choices" : choices}    
  return None

def get_plan(runner_file):
  global plans
  script = ingest_file(runner_file)
  script = preprocess(script)
  script = extract_settings(script)
  plans = parse(script)

############################################################

def create_round():
  # go through each plan, expected to have a 'type'
  # depending on type, call a randomization handler
  # add the received string to the arguments array
  # return the arguments joined with whitespace
  return ""

def randomization_handler1(plan):
  # use the type-specific diction values to create an argument
  # return the argument key=value string
  return ""

def assemble_command_line(arguments):
  # return a string with the pre, arguments, and post parts
  return ""

def run_program(command_line):
  # launch the program (capture output for verbose display)
  pass

def round_delay():
  # wait the specified seconds
  time.sleep(round_time)

def do_round():
  arguments = create_round()
  command_line = assemble_command_line(arguments)
  run_program(command_line)
  round_delay()
  


############################################################
############################################################

global app_description, verbose_mode, quiet_mode, runner_file
app_description = None
verbose_mode = None
quiet_mode = None
runner_file = ""

def get_options():
    global app_description, verbose_mode, runner_file
    app_description = "Application Runner - Apollo Lighting System v.0.0 2-0-2019"

    parser = argparse.ArgumentParser(description=app_description)
    parser.add_argument("-v", "--verbose", dest="verbose", action="store_true", help="display verbose info (False)")
    parser.add_argument("-q", "--quiet", dest="quiet", action="store_true", help="don't use terminal colors (False)")
    parser.add_argument("runner", help="runner script filename")
    args = parser.parse_args()
    verbose_mode = args.verbose
    quiet_mode = args.quiet
    runner_file = args.runner

def validate_options():
    pass

def introduction():
    ui.app_description(app_description)
    ui.report_verbose("verbose mode")
    ui.report_verbose()

def initialize():
    get_options()
    validate_options()
    tc.begin(quiet_mode)
    ui.begin(verbose_mode, quiet_mode)
    introduction()
    get_plan(runner_file)
    print plans

def loop():
    do_round()

############################################################
############################################################

if __name__ == '__main__':
    initialize()
    while True:
        try:
            loop()
        except KeyboardInterrupt:
            sys.exit("\nExiting...\n")
        except Exception:
            raise


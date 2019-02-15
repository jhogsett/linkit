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
  # strip each line
  # remove from each line anything to the right of a command mark
  # leave line out completely if empty
  return lines

def extract_settings(lines):
  # first line is command line prior to placement of random key=value pairs
  # second line is command line after placement of random pairs (can be blank)
  # third line is time in seconds to wait before going to the next round
  # remaining lines are type-specific arguments
  return lines

#num-boxes          1-5
#balls-per-box      1-4
#box-width          6-30
#background-type    0-2
#box-step           1 2 4
#refresh-time       40-120 10
#ball-time          40-80 10
#box-sequencer      seq sqs sws swc
#ball-sequencer     seq sqs sws swc
#ball-step          1-3
#use-computed-value True False

# types:
# 
# 1) integer range (second arg has a '-' that splits into two valid integers)
# 2) integer range with quantization (+ third arg has an int)
# 3) series of values if the above two are not met

def parse(script):
  # process each line
  # hand off to parsing handlers to process line
  # add returned plan to plan list 
  return [{}]

def parsing_handler1(line):
  # determine if this line should be processed; return None if not
  # process line
  # return line processed into a handler-specific dictionary
    # all need to have a 'type'
    # the type determines how to randomize later
  return {}

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


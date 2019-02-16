#!/usr/bin/python

import sys
import argparse
import terminal_colors as tc
import app_ui as ui
import os
import utils
import time
import random
import led_command as lc
import macro_compiler as mc

global plans, program, round_time
plans = []
program = ""
round_time = 0

############################################################

def ingest_file(filename):
  return utils.load_file(filename, ".run")

def preprocess(lines):
  return utils.strip_comments(lines)

def extract_settings(lines):
  global program, round_time
  program = lines[0]
  round_time = int(lines[1])
  return lines[2:]

def parse(script):
  plans = []
  for line in script:
    result = parsing_handler1(line)
    if not result:
      result = parsing_handler2(line)
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
      low = int(range[0])
      high = int(range[1])
      count = (high - low) + 1
      return {"type" : 1, "name" : name, "count" : count, "low" : low, "high" : high}
  return None

#refresh-time 40-120 10
# 2) integer range with quantization (+ third arg has an int)
def parsing_handler2(line):
  parts = line.split()
  if len(parts) == 3:
    name = parts[0]
    range = parts[1].split("-")
    if len(range) == 2:
      low = int(range[0])
      high = int(range[1])
      quant = int(parts[2])
      diff = high - low
      count = (diff / quant) + 1
      return {"type" : 2, "name" : name, "count" : count, "low" : low, "high" : high, "quant" : quant}
  return None

#ball-sequencer seq sqs sws swc
# 3) series of values if the above two are not met
def parsing_handler3(line):
  parts = line.split()
  if len(parts) > 1:
    name = parts[0]
    choices = parts[1:]
    if len(choices) > 0:
      return {"type" : 3, "name" : name, "count" : len(choices), "choices" : choices}    
  return None

def report_stats():
  permutations = 1
  for plan in plans:
    permutations *= plan["count"]
  ui.report_info("permutations: " + str(permutations))

def get_plan(runner_file):
  global plans
  script = ingest_file(runner_file)
  script = preprocess(script)
  script = extract_settings(script)
  plans = parse(script)
  report_stats()

############################################################

def create_round():
  all_arguments = {}
  for plan in plans:
    type = plan["type"]
    if type == 1:
      arguments = randomization_handler1(plan)
    elif type == 2:
      arguments = randomization_handler2(plan)
    elif type == 3:
      arguments = randomization_handler3(plan)
    all_arguments = utils.merge_dicts(all_arguments, arguments)
  return all_arguments
  
def assemble_argument(name, value):
  return name + "=" + str(value)

def randomization_handler1(plan):
  value = random.randint(plan["low"], plan["high"])
  return { plan["name"] : value }

def quantize_value(value, quant):
  return int(value/quant) * quant

def randomization_handler2(plan):
  value = random.randint(plan["low"], plan["high"])
  value = quantize_value(value, plan["quant"])
  return { plan["name"] : value }

def randomization_handler3(plan):
  choices = plan["choices"]
  count = len(choices)
  choice = random.randint(0, count-1)
  return { plan["name"] : choices[choice] }

#def assemble_command_line(arguments):
#  return pre_command_line + " " + arguments + " " + post_command_line

#def run_program(command_line):
#  ui.report_info("running: " + command_line)
#  utils.run_command(command_line)


def set_script(script_text):
    global macro_count, bytes_programmed
    macro_count = 0
    bytes_programmed = 0
    try:
        bytes = lc.command_int(script_text);
        bytes_programmed += bytes
        ui.report_verbose("programmed: " + script_text)
        ui.report_verbose_alt("bytes: " + str(bytes))
        macro_count += 1
    except ValueError as e:
      print str(e) + " - retrying"
      set_script(script_text)

def program_script(presets):
    compiled_script = ""
    compilation_succeeded = True
    mc.reset(presets)
    try:
        compiled_script = mc.compile_file(program)
    except ValueError, e:
        print
        ui.report_error("Fatal error compiling script. Reported error: ")
        ui.report_error_alt(str(e))
        compilation_succeeded = False

    if verbose_mode:
        print
        ui.report_verbose("compiled script:")
        for script_text in compiled_script:
            ui.report_verbose_alt(script_text)
    script_ok = False
    if compilation_succeeded:
        compilation_valid = mc.compilation_valid(compiled_script)
        if not mc.compilation_valid(compiled_script):
          ui.report_error("Compilation failed!")
        script_ok = compilation_valid
        if compilation_valid:
            for script_text in compiled_script:
              set_script(script_text)
            script_ok = verify_programming(compiled_script)
    return script_ok

def verify_programming(compiled_script):
  script_ok = True
  for compiled_line in compiled_script:
    macro_number = int(compiled_line.split(":")[0])
    programmed_line = lc.get_macro(macro_number)
    if programmed_line != compiled_line:
      script_ok = False
      print
      ui.report_error("Macro doesn't match:")
      print tc.green("Expected: " + compiled_line)
      print tc.red("     Got: " + programmed_line)
      print
    ui.write(tc.green('.'))
  print
  return script_ok

def run_program(arguments):
  presets = utils.merge_dicts(device_presets, arguments)
  script_ok = program_script(presets)
  if script_ok:
    lc.command(":::stp")
    lc.command("10:run")
  return script_ok

def round_delay():
  ui.report_info("Press a key to Vote:")
  choice = utils.get_input(round_time)
  if choice == " ":
    choice = "liked"
  elif choice == None:
    choice = "no vote"
  else:
    choice = "pressed: " + choice
  ui.report_info_alt(choice)
  return choice

def add_to_log(line):
  with open('runner.log', 'a') as file:
    file.write(line + '\n')

def format_arguments(arguments):
  line = ""
  for name in arguments.keys():
    line += name + "=" + str(arguments[name]) + " "
  return line

def do_round():
  arguments = create_round()
  formatted_arguments = format_arguments(arguments)
  add_to_log(program + " " + formatted_arguments)
  ui.report_info(formatted_arguments)
  if run_program(arguments):
    vote = round_delay()
    add_to_log(vote)
  else:
    add_to_log("failed")

############################################################
############################################################

global app_description, verbose_mode, quiet_mode, runner_file, device_presets
app_description = None
verbose_mode = None
quiet_mode = None
runner_file = ""
device_presets = None

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
    global device_presets
    get_options()
    validate_options()
    tc.begin(quiet_mode)
    ui.begin(verbose_mode, quiet_mode)
    introduction()

    lc.begin(verbose_mode)
    lc.stop_all()

    device_presets = lc.get_device_profile()
    num_leds = device_presets["NUM-LEDS"]
    starting_macro = device_presets["START-MACRO"]
    num_macro_chars = device_presets["NUM-MACRO-BYTES"]
    ending_macro = device_presets["END-MACRO"]
    number_of_macros = device_presets["NUM-MACROS"]
    char_buffer_size = device_presets["CHAR-BUFFER-SIZE"]
    number_of_fine_zones = device_presets["NUM-FINE-ZONES"]
    number_of_colors = device_presets["NUM-PALETTE-COLORS"]
    number_of_sequencers = device_presets["NUM-SEQUENCERS"]
    total_macro_bytes = device_presets["TOTAL-MACRO-BYTES"]
    last_macro_bytes = device_presets["LAST-MACRO-BYTES"]

    mc.begin(lc, verbose_mode, quiet_mode, device_presets, starting_macro, ending_macro, number_of_sequencers, num_macro_chars, char_buffer_size, last_macro_bytes)

def loop():
    do_round()

############################################################
############################################################

if __name__ == '__main__':
    initialize()
    get_plan(runner_file)
    while True:
        try:
            loop()
        except KeyboardInterrupt:
            sys.exit("\nExiting...\n")
        except Exception:
            raise


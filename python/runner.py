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

global plans, program, anti_plans
plans = []
program = ""
anti_plans=[]

############################################################

def ingest_file(filename):
  return utils.load_file(filename, ".run")

def preprocess(lines):
  lines = utils.strip_whitespace(lines)
  return utils.strip_comments(lines)

def validate_program(program):
  orig_program = program
  program = program.strip()
  program = utils.locate_file(program, ".mac", default_directory)
  ui.report_verbose("program found at: " + program)
  return program

def extract_settings(lines):
  global program
  program = lines[0].strip()
  program = validate_program(program)
  return lines[1:]

def parse(script):
  global plans, anti_plans
  plans = []
  anti_plans=[]
  for line in script:
    if len(line) == 0:
      continue
    if line[0] == "~":
      line = line[1:]
      result = parse_anti_plan(line)
      anti_plans.append(result)
      continue
    result = parsing_handler1(line)
    if not result:
      result = parsing_handler2(line)
    if not result:
      result = parsing_handler3(line)
    if result:
      plans.append(result)

#num-boxes 1-5
# 1) integer range (second arg has a '-' that splits into two valid integers)
def parsing_handler1(line):
  parts = line.split()
  if len(parts) == 2:
    name = parts[0]
    range = parts[1].split("-")
    if len(range) == 2:
      try:
        low = int(range[0])
        high = int(range[1])
        count = (high - low) + 1
        return {"type" : 1, "name" : name, "count" : count, "low" : low, "high" : high}
      except ValueError:
        return None
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

def parse_anti_plan(line):
  parts = line.split()
  if len(parts) > 1:
    name = parts[0]
    matches = parts[1:]
    if len(matches) > 0:
      return {"type" : 0, "name" : name, "matches" : matches}
  return None

def permutations():
  permutations = 1
  for plan in plans:
    permutations *= plan["count"]
  return permutations

def plan_entry(name, value):
  return tc.cyan(name) + " " + tc.green(str(value))

def plan_header(type, name):
  return tc.blue("[ " + type + " ]") + " " + tc.yellow(name + ":")

def anti_plan_header(type, name):
  return tc.red("[ " + type + " ]") + " " + tc.yellow(name + ":")

def plan_choices(choices):
  return " ".join(choices)

def anti_plan_matches(matches):
  return " ".join(matches)

def show_plan1(plan):
  return plan_header("INTEGER RANGE", plan["name"]) + " " + plan_entry("low:", plan["low"]) + " " + plan_entry("high:", plan["high"])

def show_plan2(plan):
  return plan_header("QUANTIZED RANGE", plan["name"]) + " " +plan_entry("low:", plan["low"]) + " " + plan_entry("high:", plan["high"]) + " " + plan_entry("quantized to:", str(plan["quant"]) + "'s")

def show_plan3(plan):
  choices = plan_choices(plan["choices"])
  return plan_header("SELECTED CHOICE", plan["name"]) + " " +plan_entry("choices:", choices)

def show_anti_plan(anti_plan):
  matches = anti_plan_matches(anti_plan["matches"])
  return anti_plan_header("ANTI PLAN", anti_plan["name"]) + " " + plan_entry("matches:", matches)

def formatted_permutations():
  return "{:,}".format(permutations())

def formatted_plan():
  formatted_plan_ = []
  for plan in plans:
    line = ""
    for key in plan.keys():
      line = line + key + ":" + str(plan[key]) + " "
    formatted_plan_.append(line)
  return "\n".join(formatted_plan_)

def formatted_anti_plan():
  formatted_anti_plan_ = []
  for anti_plan in anti_plans:
    line = ""
    for key in anti_plan.keys():
      line = line + key + ":" + str(anti_plan[key]) + " "
    formatted_anti_plan_.append(line)
  return "\n".join(formatted_anti_plan_)

def report_plan():
  ui.report_info("permutations: " + tc.green(formatted_permutations()))
  ui.report_separator()

  if show_plan:
    for plan in plans:
      if plan["type"] == 1:
        ui.write_line(show_plan1(plan))
      if plan["type"] == 2:
        ui.write_line(show_plan2(plan))
      if plan["type"] == 3:
        ui.write_line(show_plan3(plan))
    for anti_plan in anti_plans:
      ui.write_line(show_anti_plan(anti_plan))
    ui.report_separator()

def formatted_device_info():
  formatted_info_ = []
  for key in device_presets:
    line = key + ":" + str(device_presets[key])
    formatted_info_.append(line)
  return "\n".join(formatted_info_)

def log_plan():
  add_to_log("")
  add_to_log("program: " + program)
  add_to_log(formatted_device_info())
  add_to_log(formatted_plan())
  add_to_log(formatted_anti_plan())
  add_to_log("random seed: " + str(random_seed))
  add_to_log("permutations: " + formatted_permutations())

def get_plan(runner_file):
  script = ingest_file(runner_file)
  script = preprocess(script)
  script = extract_settings(script)
  parse(script)
  report_plan()
  log_plan()

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
      ui.report_error(str(e) + " - retrying")
      set_script(script_text)

def program_script(presets):
    global compiler_error
    compiled_script = ""
    compilation_succeeded = True
    mc.reset(presets)
    try:
        compiled_script = mc.compile_file(program)
        if show_script:
          ui.report_separator()
          for line in compiled_script:
            ui.report_info_alt(line)
    except ValueError, e:
        if verbose_mode:
          ui.report_separator()
          ui.report_error("Fatal error compiling script. Reported error: ")
          ui.report_error_alt(str(e))
        compiler_error = str(e)
        compilation_succeeded = False

    if verbose_mode:
        ui.report_separator()
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
              if not verbose_mode:
                ui.write(tc.yellow('.'))
            if no_verify:
              script_ok = True
              ui.report_separator()
            else:
              script_ok = verify_programming(compiled_script)
    return script_ok

def verify_programming(compiled_script):
  script_ok = True
  for compiled_line in compiled_script:
    macro_number = int(compiled_line.split(":")[0])
    programmed_line = lc.get_macro(macro_number)
    if programmed_line != compiled_line:
      script_ok = False
      ui.report_separator()
      ui.report_error("Macro doesn't match:")
      ui.write_line(tc.green("Expected: " + compiled_line))
      ui.write_line(tc.red("     Got: " + programmed_line))
      ui.report_separator()
    if not verbose_mode:
      ui.write(tc.cyan('.'))
  ui.report_separator()
  return script_ok

def record_program(arguments):
  presets = utils.merge_dicts(device_presets, arguments)
  script_ok = program_script(presets)
  return script_ok

def run_program():
    lc.command(":::stp")
    lc.command("10:run")

def stop_program():
    lc.command(":::stp")
    lc.command(":::stp")

def enter_pause_loop():
  ui.report_info_alt2("Paused - press a key to continue...")
  while True:
    if utils.get_input(1) != None:
      return

def round_delay():
  if not no_rating:
    ui.report_info_alt2("Press X to exit, P to pause, any other key to Vote:")
    choice = utils.get_input(round_time)
    if choice == "x" or choice == "X":
      raise KeyboardInterrupt
    elif choice == "p" or choice == "P":
      enter_pause_loop()
      return round_delay()
    if choice == " ":
      choice = default_choice
    elif choice == None:
      choice = "no rating"
    else:
      choice = "pressed: " + choice
    ui.report_info_alt(choice)
  else:
    ui.report_info_alt2("Displaying for " + str(round_time) + " seconds...")
    time.sleep(round_time)
    choice = "rating disabled"
  return choice

def log_filename():
  global program
  filename = os.path.basename(program)
  filename, file_extension = os.path.splitext(filename)
  log_filename = os.path.join(default_directory, program + ".runlog")
  ui.report_verbose("log path: " + log_filename)
  return log_filename

def add_to_log(line):
  with open(log_filename(), 'a') as file:
    file.write(line + '\n')

def format_argument(name, value):
  return tc.cyan(name + ":") + tc.yellow("[" + str(value) + "]")

def format_arguments(arguments):
  line = ""
  for name in arguments.keys():
    line += format_argument(name, arguments[name]) + " "
  return line

def list_arguments(arguments):
  line = ""
  for name in arguments.keys():
    line += name + "=" + str(arguments[name]) + " "
  return line

def log_arguments(arguments): 
  listed_arguments = list_arguments(arguments)
  add_to_log(program + " " + listed_arguments)

def display_arguments(arguments):
  formatted_arguments = format_arguments(arguments)
  ui.write_line(formatted_arguments)

def passes_anti_plan(arguments):
  if len(anti_plans) == 0:
    return True
  listed_arguments = list_arguments(arguments)
  for anti_plan in anti_plans:
    matches = anti_plan["matches"]
    allowed = False
    for match in matches:
      if not match in listed_arguments:
        allowed = True
        break
    if not allowed:
      ui.report_verbose_alt2("plan skipped due to anti plan: " + anti_plan["name"])
      add_to_log("skipped by anti plan: " + anti_plan["name"])
      return False
  return True

def do_round():
  arguments = create_round()
  log_arguments(arguments)

  if passes_anti_plan(arguments):
    if record_program(arguments):
      display_arguments(arguments)
      run_program()
      vote = round_delay()
      stop_program()
      add_to_log(vote)
    else:
      add_to_log("error: " + compiler_error)


############################################################
############################################################

global app_description, verbose_mode, quiet_mode, runner_file, device_presets, show_plan, show_script, no_verify, round_time, extra_verbose_mode, compiler_error, no_rating, random_seed, default_directory
app_description = None
verbose_mode = None
quiet_mode = None
runner_file = ""
device_presets = None
show_plan = None
show_script = None
no_verify = None
round_time = None
extra_verbose_mode = None
compiler_error = ""
default_choice = None
no_rating = None
random_seed = None
default_directory = None

def get_options():
    global app_description, verbose_mode, runner_file, show_plan, show_script, no_verify, round_time, extra_verbose_mode, default_choice, no_rating, random_seed

    app_description = "Auto Program Runner - Apollo Lighting System v.0.0 2-0-2019"

    parser = argparse.ArgumentParser(description=app_description)
    parser.add_argument("runner", help="runner script filename")
    parser.add_argument("-v", "--verbose", dest="verbose", action="store_true", help="display verbose info (False)")
    parser.add_argument("-q", "--quiet", dest="quiet", action="store_true", help="don't use terminal colors (False)")
    parser.add_argument("-p", "--dont-show-plan", dest="dont_show_plan", action="store_true", help="don't show the randomization plan (False)")
    parser.add_argument("-o", "--show-script", dest="show_script", action="store_true", help="show the compiled script (False)")
    parser.add_argument("-n", "--no-verify", dest="no_verify", action="store_true", help="skip the programming verification step (False)")
    parser.add_argument("-w", "--wait-time", type=int, dest="wait_time", default=30, help="wait time between rounds in seconds (30)")
    parser.add_argument("-x", "--extra-verbose", dest="extra_verbose", action="store_true", help="display extra verbose info (False)")
    parser.add_argument("-d", "--default-choice", dest="default_choice", default="skipped", help="vote log entry when hitting space bar (skipped)")
    parser.add_argument("-r", "--no-rating", dest="no_rating", action="store_true", help="don't prompt user for a rating vote (False)")
    parser.add_argument("-s", "--seed", type=int, dest="seed", default=0, help="random number generator seed value (random")

    args = parser.parse_args()
    verbose_mode = args.verbose
    quiet_mode = args.quiet
    runner_file = args.runner
    show_plan = not args.dont_show_plan
    show_script = args.show_script
    no_verify = args.no_verify
    round_time = args.wait_time
    extra_verbose_mode = args.extra_verbose
    default_choice = args.default_choice
    no_rating = args.no_rating   
    random_seed = args.seed

def validate_options():
    pass

def introduction():
    ui.app_description(app_description)
    ui.report_verbose("verbose mode")
    ui.report_verbose()

def initialize():
    global device_presets, random_seed, runner_file

    get_options()
    validate_options()

    tc.begin(quiet_mode)
    ui.begin(verbose_mode, quiet_mode)

    runner_file = runner_file.strip()
    runner_file = utils.locate_file(runner_file, ".run", get_script_directory())

    get_default_directory(runner_file)

    introduction()

    random_seed = utils.randomize(random_seed)
    ui.report_info("random seed: " + tc.green(str(random_seed)))

    lc.begin(extra_verbose_mode)
    lc.stop_all()
    utils.begin(not no_rating)

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

    mc.begin(lc, extra_verbose_mode, quiet_mode, device_presets, starting_macro, ending_macro, number_of_sequencers, num_macro_chars, char_buffer_size, last_macro_bytes)

def get_script_directory():
    return os.path.dirname(os.path.abspath(__file__))

def get_default_directory(run_file):
    global default_directory
    default_directory = os.path.dirname(os.path.abspath(run_file))
    if len(default_directory) == 0:
        default_directory = get_script_directory()
    ui.report_verbose("default directory: " + default_directory)
    return default_directory

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


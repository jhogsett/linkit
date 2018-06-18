#!/usr/bin/python

import serial 
import time
import sys
import terminal_colors as tc
import led_command as lc
import argparse
import app_ui as ui
import macro_compiler as mc
import math

global app_description, verbose_mode, debug_mode, macro_count, program, macro_run_number, presets, dryrun, bytes_programmed, show_output, show_tables
app_description = None
verbose_mode = None
debug_mode = None
macro_count = 0
programs = None
macro_run_number = None
presets = None
dryrun = None
bytes_programmed = None
show_output = None
show_tables = None

global device_profile, num_leds, starting_macro, num_macro_chars, ending_macro, number_of_macros, char_buffer_size, number_of_fine_zones, number_of_colors, number_of_sequencers
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

def get_options():
    global verbose_mode, debug_mode, program, macro_run_number, presets, dryrun, show_output, show_tables, num_macro_chars_override, starting_macro_override, ending_macro_override, char_buffer_override

    parser = argparse.ArgumentParser(description=app_description)
    parser.add_argument("program", help="program to transmit")
    parser.add_argument("presets", nargs="*", help="resolved=value presets (None)")
    parser.add_argument("-m", "--macro", type=int, dest="macro", default=10, help="macro number to run after programming (10)")
    parser.add_argument("-v", "--verbose", dest="verbose", action="store_true", help="display verbose info (False)")
    parser.add_argument("-d", "--debug", dest="debug", action="store_true", help="display verbose info (False)")
    parser.add_argument("-r", "--dryrun", dest="dryrun", action="store_true", help="process the script but don't actually program the device (False)")
    parser.add_argument("-o", "--show-output", dest="show_output", action="store_true", help="display compiled script (False)")
    parser.add_argument("-t", "--show-tables", dest="show_tables", action="store_true", help="display compilation dictionaries (False)")
    parser.add_argument("-b", "--bytes-per-macro", type=int, dest="bytes_per_macro", default=0, help="bytes per macro override (none)")
    parser.add_argument("-s", "--starting-macro", type=int, dest="starting_macro", default=0, help="starting macro override (none)")
    parser.add_argument("-e", "--ending-macro", type=int, dest="ending_macro", default=0, help="ending macro override (none)")
    parser.add_argument("-c", "--char-buffer", type=int, dest="char_buffer", default=0, help="char buffer size override (none)")

    args = parser.parse_args()
    program = args.program
    macro_run_number = args.macro
    verbose_mode = args.verbose
    debug_mode = args.debug

    num_macro_chars_override = args.bytes_per_macro
    starting_macro_override = args.starting_macro
    ending_macro_override = args.ending_macro
    char_buffer_override = args.char_buffer
    presets = args.presets
    dryrun = args.dryrun
    show_output = args.show_output
    show_tables = args.show_tables

def initialize():
    global app_description,bytes_programmed
    global device_profile, num_leds, starting_macro, num_macro_chars, ending_macro, number_of_macros, char_buffer_size, number_of_fine_zones, number_of_colors, number_of_sequencers
    app_description = "Apollo Lighting System - Macro Programmer v.2.0 6-1-2018"
    get_options()

    if not validate_options():
        sys.exit("\nExiting...\n")

    ui.begin(verbose_mode)

    bytes_programmed = 0
    lc.begin(verbose_mode)

    if dryrun:
      lc.pause()
    else:
      lc.stop_all()

    device_profile = lc.get_device_profile()
    num_leds = device_profile["NUM-LEDS"]
    starting_macro = device_profile["START-MACRO"]
    num_macro_chars = device_profile["NUM-MACRO-CHARS"]
    ending_macro = device_profile["END-MACRO"]
    number_of_macros = device_profile["NUM-MACRO-CHARS"]
    char_buffer_size = device_profile["CHAR-BUFFER-SIZE"]
    number_of_fine_zones = device_profile["NUM-FINE-ZONES"]
    number_of_colors = device_profile["NUM-PALETTE-COLORS"]
    number_of_sequencers = device_profile["NUM-SEQUENCERS"]

    if num_macro_chars_override != 0:
      num_macro_chars = num_macro_chars_override
    if starting_macro_override != 0:
      starting_macro = starting_macro_override
    if ending_macro_override != 0:
      ending_macro = ending_macro_override
    if char_buffer_override != 0:
      char_buffer_size = char_buffer_override

    all_presets = merge_two_dicts(device_profile, get_command_line_presets())
    mc.begin(lc, verbose_mode, all_presets, starting_macro, ending_macro, number_of_sequencers, num_macro_chars, char_buffer_size)
    if dryrun:
      lc.resume()

# https://stackoverflow.com/questions/38987/how-to-merge-two-dictionaries-in-a-single-expression
def merge_two_dicts(x, y):
    """Given two dicts, merge them into a new dict as a shallow copy."""
    z = x.copy()
    z.update(y)
    return z

def get_command_line_presets():
    result = {}
    for preset in presets:
      args = preset.split("=")
      result[args[0]] = args[1]
    return result

#def get_device_presets():
#    return {
#      "NUM-LEDS": num_leds,
#      "NUM-MACROS": number_of_macros,
#      "NUM-SEQUENCERS": number_of_sequencers,
#      "START-MACRO": starting_macro,
#      "END-MACRO": ending_macro,
#      "NUM-MACRO-CHARS": num_macro_chars,
#      "CHAR-BUFFER-SIZE": char_buffer_size,
#      "NUM-SEQUENCERS": number_of_sequencers,
#      "NUM-FINE-ZONES": number_of_fine_zones,
#      "NUM-PALETTE-COLORS": number_of_colors
#    }

# returns True if they're valid
def validate_options():
    errors = False
    return not errors

def set_macro(macro_num, macro_text, expected_bytes):
    global macro_count, bytes_programmed

    bytes = 0
    try:
        bytes = lc.set_macro(macro_num, macro_text, expected_bytes, debug_mode)
        bytes_programmed += bytes

    except StandardError, e:
      print str(e) + " - retrying"
      try:
        lc.set_macro(macro_num, macro_text, expected_bytes, debug_mode)        
      except StandardError, e:
        sys.exit("\nUnable to program macros. Macro file may be corrupt.")

    lc.command_str(str(bytes/2) + ":pal:flu")
    macro_count += 1

    if not debug_mode:                                             
        ui.write(tc.green('.'))

def set_script(script_text):
    global macro_count, bytes_programmed
    try:
        bytes = lc.command_int(script_text);
        bytes_programmed += bytes

        ui.report_verbose("bytes programmed: " + str(bytes))

        lc.command_str(str(bytes % number_of_colors) + ":pal:mir:flu")
        macro_count += 1

        if not debug_mode:
            ui.write(tc.green('.'))

    except StandardError, e:
      print str(e) + " - retrying"
      set_script(script_text)

def import_file(program_name):
    script = []
    show_comments = True
    program_name = "./" + program_name

    if not program_name.endswith(".mac"):
        program_name = program_name  + ".mac"

    file = open(program_name, "r")
    for line in file:
        line = line.strip()
        if len(line) == 0:
            continue
        if line[0] == "#":
            if show_comments:
                print tc.yellow(line[1:].strip())
            continue
        else:
            if show_comments:
                print
                show_comments = False
        script.append(line)
    return script

def get_list_width(list):
  result = -1
  for item in list:
    length = len(str(item))
    if length > result:
      result = length
  return result

def print_table(description, table):
    ui.report_info_alt(description + ":")
    keys = table.keys()
    values = table.values()
    keys_width = get_list_width(keys)
    values_width = get_list_width(values)    

    for key in sorted(table.iterkeys()):
        key_len = len(str(key))
        key_diff = keys_width - key_len
        value = table[key]
        filler = " " * key_diff
        ui.info_entry_alt(filler + str(key), str(value))

def program_macros(program_name):
    compiled_script = ""
    try:
      compiled_script = mc.compile_file(program_name)
    except ValueError, e:
      ui.report_error("Failed to compiled script. Reported error: ")
      ui.report_error_alt(str(e))
      ui.report_error("compiled script:")
      print_script(mc.get_saved_bad_script())
      return

    if verbose_mode:
        ui.report_verbose("compiled script:")
        for script_text in compiled_script:
            ui.report_verbose_alt(script_text)

    if show_output and not verbose_mode:
        print
        ui.report_info("compiled script:")
        for script_text in compiled_script:
            ui.report_info_alt(script_text)
        print

    if show_tables:
      print_table("Presets", mc.get_presets())
      print_table("Resolved Values", mc.get_resolved())
      print_table("Unresolved Macros", mc.get_unresolved())
      print_table("Final Macro Numbers", mc.get_final_macro_numbers())
      print_table("Macros", mc.get_macros())

    if not mc.compilation_valid(compiled_script):
      ui.report_error("Compilation failed!")
      if not verbose_mode:
        print_script(compiled_script)
      sys.exit("\nExiting...\n")

    if not dryrun:
      for script_text in compiled_script:
        set_script(script_text) 

def print_script(script_lines):
  for script_text in script_lines:
    ui.report_warn(script_text)

# --------------------------------------------------------------------------
    
def introduction():
    ui.app_description(app_description)

    ui.report_verbose("verbose mode")
    ui.report_verbose("debug mode: " + str(debug_mode))
    ui.report_verbose("macro chars override: " + str(num_macro_chars_override))
    ui.report_verbose("start macro override: " + str(starting_macro_override))
    ui.report_verbose("end macro override: " + str(ending_macro_override))
    ui.report_verbose("char buffer override: " + str(char_buffer_override))
    ui.report_verbose()

    ui.report_info(ui.intro_entry("Number of LEDs", num_leds))
    ui.report_info(ui.intro_entry("Number of macros", (ending_macro - starting_macro) + 1))
    ui.report_info(ui.intro_entry("Number of sequencers", number_of_sequencers))
    ui.report_info(ui.intro_entry("Bytes per macro", num_macro_chars))
    ui.report_info(ui.intro_entry("First macro", starting_macro))
    ui.report_info(ui.intro_entry("Last macro", ending_macro))
    ui.report_info(ui.intro_entry("Char buffer size", char_buffer_size))
    ui.report_info("program: " + tc.green(program))
    print
   
    if dryrun:
      ui.report_warn("Dry-run enabled. The device will not be programmed.")
      print

def summary():
  total_macros = (ending_macro - starting_macro) + 1
  used_macros = macro_count
  remaining_macros = total_macros - macro_count
  used_macros_percent = (100.0 * used_macros / total_macros)
  remaining_macros_percent = (100.0 * remaining_macros / total_macros)
  remaining_sequencers = mc.remaining_sequencers()
  used_sequencers = number_of_sequencers - remaining_sequencers
  remaining_sequencers_percent = round(100.0 * remaining_sequencers / number_of_sequencers)
  used_sequencers_percent = round(100.0 * used_sequencers / number_of_sequencers)
  total_macro_bytes = 1024
  remaining_macro_bytes = total_macro_bytes - bytes_programmed
  used_bytes_percent = round(100.0 * bytes_programmed / total_macro_bytes)
  remaining_bytes_percent = round(100.0 * remaining_macro_bytes / total_macro_bytes)
  bytes_used_per_macro = round(bytes_programmed / used_macros) if int(used_macros) > 0 else 0
  bytes_used_per_macro_percent = round(100.0 * bytes_used_per_macro / num_macro_chars)
  print
  print
  print tc.green("%d Macros successfully programmed" % macro_count)
  print tc.yellow("%d Macros remaining (%d%%)" % (remaining_macros, remaining_macros_percent))
  print tc.yellow("%d Used / %d free macro bytes (%d%% / %d%%)" % (bytes_programmed, remaining_macro_bytes, used_bytes_percent, remaining_bytes_percent))
  print tc.yellow("%d Used / %d free sequencers (%d%% / %d%%)" % (used_sequencers, remaining_sequencers, used_sequencers_percent, remaining_sequencers_percent))
  print tc.cyan("%d Bytes per macro (%d%% efficiency)" % (bytes_used_per_macro, bytes_used_per_macro_percent))
  print

def upload_programs():
    program_macros(program)

def run_default_macro():
    if dryrun:
        pass
    else:
        resolved = mc.get_resolved()
        final_macro_numbers = mc.get_final_macro_numbers()
        if "%run-macro" in resolved:
          run_macro_name = resolved["%run-macro"]
          if run_macro_name in resolved:
            orig_macro_number = int(resolved[run_macro_name][1:-1]) # remove '
            if orig_macro_number in final_macro_numbers:
              run_macro_number = final_macro_numbers[orig_macro_number]
              ui.report_verbose("Running macro: " + run_macro_name + "(" + str(run_macro_number) + ")")
              lc.run_macro(run_macro_number)
            else:
              ui.report_verbose("Skipping run macro: " + str(orig_macro_number) + " (not found)")
          else:
            ui.report_verbose("Skipping run macro: " + str(run_macro_name) + " (not found)")
        else:
          lc.run_macro(macro_run_number)

############################################################################

def setup():
    initialize()
    introduction()

def loop():
    upload_programs()
    run_default_macro()
    summary()
    sys.exit()

if __name__ == '__main__':
    setup()
    while True:
#        try:
       loop()
#        except KeyboardInterrupt:
#            sys.exit("\nExiting...\n")
#        except Exception:
#            raise


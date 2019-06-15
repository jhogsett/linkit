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
import progmac_utils as pu
import utils

global app_description, verbose_mode, debug_mode, macro_count, program, macro_run_number, presets, dryrun, bytes_programmed, show_output, show_tables, quiet_mode, allow_mutability, no_led_show
app_description = None
verbose_mode = None
debug_mode = None
macro_count = 0
program = None
macro_run_number = None
presets = None
dryrun = None
bytes_programmed = None
show_output = None
show_tables = None
quiet_mode = None
allow_mutability = None
no_led_show = None

global device_profile, num_leds, starting_macro, num_macro_bytes, ending_macro, number_of_macros, char_buffer_size
global number_of_fine_zones, number_of_colors, number_of_sequencers, show_preprocessed
device_profile = None
num_leds = None
starting_macro = None
num_macro_bytes = None
ending_macro = None
number_of_macros = None
char_buffer_size = None
number_of_fine_zones = None
number_of_colors = None
number_of_sequencers = None
print_macros = None
show_preprocessed = None

def get_options():
    global verbose_mode, debug_mode, program, macro_run_number, presets, dryrun, show_output, show_tables, num_macro_bytes_override, starting_macro_override, ending_macro_override, char_buffer_override, quiet_mode, allow_mutability
    global print_macros, no_led_showi, show_preprocessed
    parser = argparse.ArgumentParser(description=app_description)
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
    parser.add_argument("-p", "--print-macros", dest="print_macros", action="store_true", help="print current macros on device (False)")
    parser.add_argument("-q", "--quiet", dest="quiet", action="store_true", help="don't use terminal colors (False)")
    parser.add_argument("-a", "--allow-mutability", dest="mutability", action="store_true", help="allow variable values to be changed (False)")
    parser.add_argument("-n", "--no-led-show", dest="no_led_show", action="store_true", help="don't write to the display while programming (False)")
    parser.add_argument("-w", "--show-preprocessed", dest="show_preprocessed", action="store_true", help="Show preprocessed script (False)")
    parser.add_argument("program", nargs="?", help="program to transmit")
    parser.add_argument("presets", nargs=argparse.REMAINDER, help="resolved=value presets (None)")

    args = parser.parse_args()
    program = args.program
    macro_run_number = args.macro
    verbose_mode = args.verbose
    debug_mode = args.debug
    quiet_mode = args.quiet
    num_macro_bytes_override = args.bytes_per_macro
    starting_macro_override = args.starting_macro
    ending_macro_override = args.ending_macro
    char_buffer_override = args.char_buffer
    presets = args.presets
    dryrun = args.dryrun
    show_output = args.show_output
    show_tables = args.show_tables
    print_macros = args.print_macros
    allow_mutability = args.mutability
    no_led_show = args.no_led_show
    show_preprocessed = args.show_preprocessed

def initialize():
    global app_description, bytes_programmed
    global device_profile, num_leds, starting_macro, num_macro_bytes, ending_macro, number_of_macros, char_buffer_size, number_of_fine_zones, number_of_colors, number_of_sequencers, last_macro_bytes, total_macro_bytes
    app_description = "Apollo Lighting System - Macro Programmer v.3.0 4-0-2018"
    get_options()

    if not validate_options():
        sys.exit("\nExiting...\n")

    tc.begin(quiet_mode)
    ui.begin(verbose_mode, quiet_mode)

    bytes_programmed = 0
    lc.begin(False) #verbose_mode)

    if dryrun:
      lc.pause()
    else:
      lc.stop_all()

    device_profile = lc.get_device_profile()
    num_leds = device_profile["NUM-LEDS"]
    starting_macro = device_profile["START-MACRO"]
    total_macro_bytes = device_profile["TOTAL-MACRO-BYTES"]
    num_macro_bytes = device_profile["NUM-MACRO-BYTES"]
    last_macro_bytes = device_profile["LAST-MACRO-BYTES"]
    ending_macro = device_profile["END-MACRO"]
    number_of_macros = device_profile["NUM-MACRO-BYTES"]
    char_buffer_size = device_profile["CHAR-BUFFER-SIZE"]
    number_of_fine_zones = device_profile["NUM-FINE-ZONES"]
    number_of_colors = device_profile["NUM-PALETTE-COLORS"]
    number_of_sequencers = device_profile["NUM-SEQUENCERS"]

    if num_macro_bytes_override != 0:
      num_macro_bytes = num_macro_bytes_override
    if starting_macro_override != 0:
      starting_macro = starting_macro_override
    if ending_macro_override != 0:
      ending_macro = ending_macro_override
    if char_buffer_override != 0:
      char_buffer_size = char_buffer_override

    all_presets = utils.merge_dicts(device_profile, get_command_line_presets())
    mc.begin(lc, verbose_mode, quiet_mode, all_presets, starting_macro, ending_macro, number_of_sequencers, num_macro_bytes, char_buffer_size, last_macro_bytes, allow_mutability)
    if dryrun:
      lc.resume()

def get_command_line_presets():
    result = {}
    for preset in presets:
      args = preset.split("=")
      if len(args) < 2:
        ui.report_error("command line preset '" + preset + "' is not a key=value pair - ignored")
      else:
        result[args[0]] = args[1]
    return result

# returns True if they're valid
def validate_options():
    errors = False
    if not print_macros:
      if len(program) == 0:
        ui.report_error("Must specify a progam to upload")
        errors = True
    return not errors

def set_script(script_text):
    global macro_count, bytes_programmed
    try:
        bytes = lc.command_int(script_text);
        bytes_programmed += bytes
        ui.report_verbose("programmed: " + script_text)
        ui.report_verbose_alt("bytes: " + str(bytes))
        if not no_led_show:
            lc.command_str(str(bytes % number_of_colors) + ":pal:mir:flu")
        macro_count += 1
        if not debug_mode:
            if not verbose_mode:
                ui.write(tc.green('.'))
        else:
            macro_number = int(script_text.split(":")[0])
            macro, carry_over_macro = lc.get_macro(macro_number)
            print macro
    except ValueError as e:
      print str(e) + " - retrying"
      set_script(script_text)

#def import_file(program_name):
#    script = []
#    show_comments = True
#    program_name = utils.locate_file(program_name, ".mac")
#    file = open(program_name, "r")
#    for line in file:
#        line = line.strip()
#        if len(line) == 0:
#            continue
#        if line[0] == "#":
#            if show_comments:
#                print tc.yellow(line[1:].strip())
#            continue
#        else:
#            if show_comments:
#                print
#                show_comments = False
#        script.append(line)
#    return script

def program_macros(program_name):
    compiled_script = ""
    ui.report_info_header("1. Compiling ")
    compilation_succeeded = True
    try:
        compiled_script = mc.compile_file(program_name)
    except ValueError, e:
        print
        ui.report_error("Fatal error compiling script. Reported error: ")
        ui.report_error_alt(str(e))
        ui.report_error("more error details:")
        pu.print_script(mc.get_saved_bad_script())
        compilation_succeeded = False
    print

    if verbose_mode:
        ui.report_verbose("compiled script:")
        for script_text in compiled_script:
            ui.report_verbose_alt3(script_text)

    script_ok = False
    if compilation_succeeded:
        compilation_valid = mc.compilation_valid(compiled_script)
        if not mc.compilation_valid(compiled_script):
          print
          ui.report_error("Compilation failed!")
          if not verbose_mode:
            print
            pu.print_script(compiled_script)

        script_ok = compilation_valid
        if compilation_valid:
          if not dryrun:
            ui.report_info_header("2. Recording ")
            if verbose_mode:
              print
            for script_text in compiled_script:
              set_script(script_text) 
            print
            ui.report_info_header("3. Verifying ")
            script_ok = pu.verify_programming(compiled_script)

    if show_tables:
      print
      pu.print_table("Presets", mc.get_presets())
      pu.print_table("Includes", mc.get_includes())
      pu.print_table("Resolved Values", mc.get_resolved())
      pu.print_table("Unresolved Macros", mc.get_unresolved())
      pu.print_table("Final Macro Numbers", mc.get_final_macro_numbers())
      pu.print_table("Macros", mc.get_macros())
      pu.print_table("Translation", mc.get_translation())

    if show_preprocessed and not verbose_mode:
        print
        ui.report_info("preprocessed script:")
        preprocessed = mc.get_preprocessed()
        for line in preprocessed:
            ui.report_info_alt(line)

    if show_output and not verbose_mode:
        print
        ui.report_info("compiled script:")
        for script_text in compiled_script:
            ui.report_info_alt3(script_text)

    return script_ok

# --------------------------------------------------------------------------
    
def introduction():
    ui.app_description(app_description)

    ui.report_verbose("verbose mode")
    ui.report_verbose("debug mode: " + str(debug_mode))
    ui.report_verbose("macro bytes override: " + str(num_macro_bytes_override))
    ui.report_verbose("start macro override: " + str(starting_macro_override))
    ui.report_verbose("end macro override: " + str(ending_macro_override))
    ui.report_verbose("char buffer override: " + str(char_buffer_override))
    ui.report_verbose()

    ui.report_info(ui.intro_entry("Number of LEDs", num_leds))
    ui.report_info(ui.intro_entry("Number of macros", (ending_macro - starting_macro) + 1))
    ui.report_info(ui.intro_entry("Number of sequencers", number_of_sequencers))
    ui.report_info(ui.intro_entry("Bytes per macro", num_macro_bytes))
    ui.report_info(ui.intro_entry("First macro", starting_macro))
    ui.report_info(ui.intro_entry("Last macro", ending_macro))
    ui.report_info(ui.intro_entry("Char buffer size", char_buffer_size))
    if not print_macros:
      ui.report_info("program: " + tc.green(program))
      if len(presets) > 0:
        for preset in presets:
          ui.report_info("command-line preset: " + tc.yellow(preset))
    print
   
    if dryrun:
      ui.report_warn("Dry-run enabled. The device will not be programmed.")
      print

def summary():
  total_macros = (ending_macro - starting_macro) + 1
  # count the number of wasted macros
  wasted_macros = 0
  for key in mc.get_final_macro_numbers():
    if "-" in str(key):
      wasted_macros += 1
  wasted_macros_percent = (100.0 * wasted_macros / total_macros)
  used_macros = macro_count + wasted_macros
  remaining_macros = total_macros - used_macros
  used_macros_percent = (100.0 * used_macros / total_macros)
  remaining_macros_percent = (100.0 * remaining_macros / total_macros)
  remaining_sequencers = mc.remaining_sequencers()
  used_sequencers = number_of_sequencers - remaining_sequencers
  remaining_sequencers_percent = round(100.0 * remaining_sequencers / number_of_sequencers)
  used_sequencers_percent = round(100.0 * used_sequencers / number_of_sequencers)
  total_bytes_programmed = used_macros * num_macro_bytes

  if ending_macro in mc.get_final_macro_numbers().keys():
    #reduce by the missing bytes in the ending macro
    total_bytes_programmed -= (num_macro_bytes - last_macro_bytes)

  remaining_macro_bytes = total_macro_bytes - total_bytes_programmed
  used_bytes_percent = round(100.0 * total_bytes_programmed / total_macro_bytes)
  remaining_bytes_percent = round(100.0 * remaining_macro_bytes / total_macro_bytes)
  bytes_used_per_macro = round(bytes_programmed / used_macros) if int(used_macros) > 0 else 0
  bytes_used_per_macro_percent = round(100.0 * bytes_used_per_macro / num_macro_bytes)

  print
  print tc.green("%d Macros successfully programmed" % macro_count)
  print
  print tc.yellow("%d Used / %d free macros  (%d%% / %d%%)" % (used_macros, remaining_macros, used_macros_percent, remaining_macros_percent))
  print tc.yellow("%d Used / %d free macro bytes (%d%% / %d%%)" % (total_bytes_programmed, remaining_macro_bytes, used_bytes_percent, remaining_bytes_percent))
  print tc.yellow("%d Used / %d free sequencers (%d%% / %d%%)" % (used_sequencers, remaining_sequencers, used_sequencers_percent, remaining_sequencers_percent))
  print tc.cyan("%d Bytes per macro (%d%% efficiency)" % (bytes_used_per_macro, bytes_used_per_macro_percent))
  print tc.cyan("%d Carry over macros (%d%% of total)" % (wasted_macros, wasted_macros_percent))
  print

def upload_programs():
    return program_macros(program)

def run_default_macro():
    if dryrun:
        pass
    else:
        resolved = mc.get_resolved()
        final_macro_numbers = mc.get_final_macro_numbers()
        if "%play-macro" in resolved:
          run_macro_name = resolved["%play-macro"]
          if run_macro_name in resolved:
            orig_macro_number = resolved[run_macro_name]
            if "'" in str(orig_macro_number):
              orig_macro_number = int(orig_macro_number[1:-1]) # remove '
            if orig_macro_number in final_macro_numbers:
              run_macro_number = final_macro_numbers[orig_macro_number]
              ui.report_info("Running macro: " + run_macro_name + " (" + str(run_macro_number) + ")")
              lc.run_macro(run_macro_number)
            else:
              ui.report_verbose("Skipping run macro: " + str(orig_macro_number) + " (not found)")
          else:
            ui.report_verbose("Skipping run macro: " + str(run_macro_name) + " (not found)")
        else:
          lc.run_macro(macro_run_number)

#def print_device_macros():
#  macros = lc.get_macros()
#  ui.report_info("Macros on device:")
#  for macro in macros:
#    ui.report_info_alt(macro)

############################################################################

def setup():
    initialize()
    introduction()

def loop():
    if upload_programs():
        run_default_macro()
    summary()
    sys.exit()

if __name__ == '__main__':
    setup()

    if print_macros:
      pu.print_device_macros()
      sys.exit()

    while True:
#        try:
       loop()
#        except KeyboardInterrupt:
#            sys.exit("\nExiting...\n")
#        except Exception:
#            raise


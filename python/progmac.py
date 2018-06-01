#!/usr/bin/python

import serial 
import time
import sys
import terminal_colors as tc
import led_command as lc
import argparse
import app_ui as ui
import macro_compiler as mc

global app_description, verbose_mode, debug_mode, legacy_mode, num_leds, macro_count, programs, macro_run_number
app_description = None
verbose_mode = None
debug_mode = None
legacy_mode = None
macro_count = 0
num_leds = None
programs = None
macro_run_number = None

def get_options():
    global verbose_mode, debug_mode, programs, macro_run_number, starting_macro, num_macro_chars, ending_macro

    parser = argparse.ArgumentParser(description=app_description)
    parser.add_argument("programs", metavar="P", nargs="+", help="one or more programs to transmit")
    parser.add_argument("-m", "--macro", type=int, dest="macro", default=10, help="macro number to run after programming (10)")
    parser.add_argument("-v", "--verbose", dest="verbose", action="store_true", help="display verbose info (False)")
    parser.add_argument("-d", "--debug", dest="debug", action="store_true", help="display verbose info (False)")
    parser.add_argument("-l", "--legacy", dest="legacy", action="store_true", help="use legacy .mac file format (False)")

    args = parser.parse_args()
    programs = args.programs
    macro_run_number = args.macro
    verbose_mode = args.verbose
    debug_mode = args.debug
    legacy_mode = args.legacy
    starting_macro = 10
    num_macro_chars = 25
    ending_macro = 50

def initialize():
    global app_description, num_leds, starting_macro, num_macro_chars, ending_macro
    app_description = "Apollo Lighting System - Macro Programmer v.1.0 1-1-2018"
    get_options()
    if not validate_options():
        sys.exit("\nExiting...\n")
    lc.begin(verbose_mode)
    num_leds = lc.get_num_leds()
    ui.begin(verbose_mode)
    starting_macro = lc.get_first_eeprom_macro()
    num_macro_chars - lc.get_num_macro_chars()
    ending_macro = starting_macro + (1024 / num_macro_chars)
    mc.begin(verbose_mode, get_device_presets(), starting_macro, ending_macro)
    lc.attention()
    lc.stop_all()
    lc.command("cnt")

def get_device_presets():
  return {
    "NUM-LEDS": num_leds
  }

# returns True if they're valid
def validate_options():
    errors = False
    return not errors

def set_macro(macro_num, macro_text, expected_bytes):
    global macro_count

    try:
        bytes = lc.set_macro(macro_num, macro_text, expected_bytes, debug_mode)

    except StandardError, e:
      print str(e) + " - retrying"
      try:
        lc.set_macro(macro_num, macro_text, expected_bytes, debug_mode)        
      except StandardError, e:
        sys.exit("\nUnable to program macros. Macro file may be corrupt.")

    lc.command_str("grn:flu")                                 
    macro_count += 1

    if not debug_mode:                                             
        ui.write(tc.green('.'))

def set_script(script_text):
    global macro_count
    try:
        bytes = lc.command_int(script_text);

        ui.report_verbose("bytes programmed: " + str(bytes))

        lc.command_str("grn:flu")
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

def legacy_program_macros(program_name):
    script = import_file(program_name)
    for line in script:
        words = line.split(";")
        macro_num = int(words[0])
        macro_text = words[1].strip()
        expected_bytes = 0
        if len(words) > 2:
          expected_bytes = int(words[2])
        set_macro(macro_num, macro_text, expected_bytes)

def program_macros(program_name):
    compiled_script = mc.compile_file(program_name)

    if verbose_mode:
        ui.report_verbose("compiled script:")
        for script_text in compiled_script:
            ui.report_verbose(script_text)

    for script_text in compiled_script:
        set_script(script_text) 

# --------------------------------------------------------------------------
    
def introduction():
    ui.app_description(app_description)

    ui.report_verbose("verbose mode")
    ui.report_verbose("debug_mode: " + str(debug_mode))
    ui.report_verbose("legacy_mode: " + str(legacy_mode))
    ui.report_verbose()

    ui.report_info(ui.intro_entry("Number of LEDs", num_leds))
    ui.report_info(ui.intro_entry("Number of macro chars", num_macro_chars))
    ui.report_info(ui.intro_entry("First EEPROM macro", starting_macro))
    ui.report_info(ui.intro_entry("Last EEPROM macro", ending_macro))
    ui.report_info("programs: " + tc.green(",".join(programs)))
    print

def summary():
    print
    print
    print tc.green(str(macro_count) + " macros successfully programmed\n")
    print

def upload_programs():
    if legacy_mode:
        for program in programs:
            legacy_program_macros(program)
    else:
        # to support multiple compiled macros, need to keep track of used macro numbers
        program_macros(programs[0])

def run_default_macro():
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
        try:
            loop()
        except KeyboardInterrupt:
            sys.exit("\nExiting...\n")
        except Exception:
            raise


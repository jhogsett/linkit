#!/usr/bin/python

import serial 
import time
import sys
import terminal_colors as tc
import led_command as lc
import argparse
import app_ui as ui

global app_description, verbose_mode, debug_mode, num_leds, macro_count, programs, macro_run_number
app_description = None
verbose_mode = None
debug_mode = None
macro_count = 0
num_leds = None
programs = None
macro_run_number = None

def get_options():
    global verbose_mode, debug_mode, programs, macro_run_number

    parser = argparse.ArgumentParser(description=app_description)
    parser.add_argument("programs", metavar="P", nargs="+", help="one or more programs to transmit")
    parser.add_argument("-m", "--macro", type=int, dest="macro", default=10, help="macro number to run after programming (10)")
    parser.add_argument("-v", "--verbose", dest="verbose", action="store_true", help="display verbose info (False)")
    parser.add_argument("-d", "--debug", dest="debug", action="store_true", help="display verbose info (False)")

    args = parser.parse_args()
    programs = args.programs
    macro_run_number = args.macro
    verbose_mode = args.verbose
    debug_mode = args.debug

def initialize():
    global app_description, num_leds
    app_description = "Apollo Lighting System - Macro Programmer v.1.0 1-1-2018"
    get_options()
    if not validate_options():
        sys.exit("\nExiting...\n")
    lc.begin(verbose_mode)
    ui.begin(verbose_mode)
    lc.attention()
    lc.stop_all()
    num_leds = lc.get_num_leds()
    lc.command("cnt")

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

def program_macros(program_name):
    program_name = "./" + program_name

    if not program_name.endswith(".mac"):
        program_name = program_name  + ".mac"

    file = open(program_name, "r")
    for line in file: 
        line = line.strip()

        if len(line) == 0:
            continue

        if line[0] == "#":
            continue

        words = line.split(";")
        macro_num = int(words[0])
        macro_text = words[1].strip()
        expected_bytes = 0
        if len(words) > 2:
          expected_bytes = int(words[2])
        set_macro(macro_num, macro_text, expected_bytes)
    
def introduction():
    ui.app_description(app_description)

    ui.report_verbose("verbose mode")
    ui.report_verbose("debug_mode: " + str(debug_mode))
    ui.report_verbose()

    ui.report_info(ui.intro_entry("Number of LEDs", num_leds))
    ui.report_info("programs: " + tc.green(",".join(programs)))
    print

def summary():
    print
    print
    print tc.green(str(macro_count) + " macros successfully programmed\n")
    print

def upload_programs():
    for program in programs:
        program_macros(program)

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


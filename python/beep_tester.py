#!/usr/bin/python

import sys
import argparse
import os
#import time
#import datetime

import terminal_colors as tc
import app_ui as ui
import utils 
import led_command as lc

# ----------------------------------------

music = 1.059463094359295
duration = 1000
start = 309 #309
end = 4500

def begin():
    lc.begin(verbose_mode);




def run():
    # for n in range(400, 4400, 5):
    # for n in range(1800, 2000, 1):
    # for n in range(2300, 2500, 3):
    # for n in range(2900, 2950, 1):
    # for n in range(3100, 3500, 3):

    freq = start
    while freq < end:
        print int(freq)
        lc.command(str(int(freq)) + "," + str(duration) + ":ton")
        freq = freq * music

# ----------------------------------------

# ========================================

global app_description, verbose_mode, quiet_mode
app_description = None
verbose_mode = None
quiet_mode = None

def get_options():
    global app_description, verbose_mode
    app_description = "(application template) - Apollo Lighting System v.0.1 1-0-2019"

    parser = argparse.ArgumentParser(description=app_description)
    parser.add_argument("-v", "--verbose", dest="verbose", action="store_true", help="display verbose info (False)")
    parser.add_argument("-q", "--quiet", dest="quiet", action="store_true", help="don't use terminal colors (False)")
    args = parser.parse_args()
    verbose_mode = args.verbose
    quiet_mode = args.quiet

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
    begin()
    introduction()

def loop():
    run()

# ========================================
# ========================================

if __name__ == '__main__':
    initialize()
    try:
        loop()
    except KeyboardInterrupt:
        sys.exit("\nExiting...\n")
    except Exception:
        raise


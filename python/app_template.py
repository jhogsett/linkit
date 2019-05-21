#!/usr/bin/python

import sys
import argparse
import os
#import time
#import datetime

import terminal_colors as tc
import app_ui as ui
import utils 

# ----------------------------------------

def begin():
    pass

def run():
    pass

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


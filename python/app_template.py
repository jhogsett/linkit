#!/usr/bin/python

import sys
import argparse
import terminal_colors as tc
import app_ui as ui

global app_description, verbose_mode
app_description = None
verbose_mode = None

def get_options():
    global app_description, verbose_mode
    app_description = "(application template) - Apollo Lighting System v.0.0 12-0-18"

    parser = argparse.ArgumentParser(description=app_description)
    parser.add_argument("-v", "--verbose", dest="verbose", action="store_true", help="display verbose info (False)")
    args = parser.parse_args()
    verbose_mode = args.verbose

def validate_options():
    pass

def introduction():
    ui.app_description(app_description)

    ui.report_verbose("verbose mode")
    ui.report_verbose()

def initialize():
    get_options()
    validate_options()
    ui.begin(verbose_mode)
    introduction()

def loop():
    pass

if __name__ == '__main__':
    initialize()
    while True:
        try:
            loop()
        except KeyboardInterrupt:
            sys.exit("\nExiting...\n")
        except Exception:
            raise


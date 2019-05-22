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

import multicast as mc
import keyboard as kb
import remote_settings as settings

# ----------------------------------------

def begin():
    lc.begin(verbose_mode)
    mc.begin("remote", verbose_mode)
    kb.begin(key_callback, verbose_mode)

    lc.stop_all()
    lc.command("1:cnt:4:cnt")

def run():
    while True:
        kb.poll_once()

def key_callback(key, long_press):
    button_set = settings.long_press if long_press else settings.short_press
    button = button_set[key-1]

    if "macro" in button:
        do_macro(button["macro"])
        kb.long_beep()

    if "color" in button:
        do_color(button["color"])
        kb.alt_beep()

    if "cmd" in button:
        do_cmd(button["cmd"])
        kb.short_beep()

    if "raw" in button:
        do_raw(button["raw"])
        kb.long_beep()

# ----------------------------------------

def do_cmd(cmd):
    command = "3:pau:" + cmd + ":3:cnt"
    mc.broadcast(command)

def do_color(color):
    command = "2:pau:" + color + ":flo:1:cnt:flu"
    mc.broadcast(command)

def do_macro(macro):
    command = "1:pau:2:cnt:" + str(macro) + ":run"
    mc.broadcast(command)

def do_raw(stop):
    command = stop
    mc.broadcast(command)

# ========================================

global app_description, verbose_mode, quiet_mode
app_description = None
verbose_mode = None
quiet_mode = None

def get_options():
    global app_description, verbose_mode
    app_description = "UDP Remote Control - Apollo Lighting System v.0.1 5-0-2019"

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


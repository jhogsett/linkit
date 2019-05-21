#!/usr/bin/python

import sys
import argparse
import os
import time
#import datetime

import terminal_colors as tc
import app_ui as ui
import utils 
import led_command as lc

poll_frequency = 20
long_press = 50
too_short_press = 3
short_press_tone = 1800
short_press_duration = 50
long_press_tone = 1700
long_press_duration = 100

# ----------------------------------------

def begin():
    lc.begin(verbose_mode);
    lc.stop_all()
    lc.command("4,0:cfg:1:cnt:3:cnt")
    lc.command_str("3,-1,0:key:0:set:" + str(long_press_tone) + "," + str(long_press_duration) + ":ton")
    #lc.command("3,-1,-1:key")

def short_beep():
    lc.command(str(short_press_tone) + "," + str(short_press_duration) + ":ton")

def long_beep():
    lc.command(str(long_press_tone) + "," + str(long_press_duration) + ":ton")

num_keys = 16
num_rows = 4
num_cols = 4
global keys
keys = [False for i in range(num_keys)]
colors = ["blu", "blu", "blu", "blu", "grn", "grn", "grn", "grn", "yel", "yel", "yel", "yel", "red", "red", "red", "red"]
effects = ["" for n in range(num_keys)]
positions = [0, 2, 4, 6, 8, 10, 12, 14, 1, 3, 5, 7, 9, 11, 13, 15]
column_selected = [False, False, False, False]
no_effect = ""

def set_column_effect(column, effect):
    for row in range(num_rows):
        index = (row * num_cols) + column
        effects[index] = effect

def set_column_blink(column):
    for row in range(num_rows):
        index = (row * num_cols) + column
        effects[index] = "bl" + str(column + 1)

def clear_column(column):
    column_selected[column] = False
    for row in range(num_rows):
        index = (row * num_cols) + column
        keys[index] = False
        effects[index] = no_effect

def on_command_key(key, long_press):
    column = key - 17

    if long_press:
        if column_selected[column]:
            column_selected[column] = False
            set_column_effect(column, no_effect)
        else:
            column_selected[column] = True
            set_column_blink(column)
    else:
        clear_column(column)
        long_beep()
    render()

def render():
#    lc.attention(False);
    lc.command("::pau")
    lc.push_command("era:")
    for i in range(num_keys):
        if keys[i]:
            lc.push_command(str(positions[i]) + ":pos:" + colors[i] + ":" + effects[i] + ":rst:")
    lc.push_command("1,1:flu")
    lc.push_command()
    lc.command("1:cnt")

def on_key(key, long_press):
    global keys
    if key > 16:
        return on_command_key(key, long_press)
    key -= 1
    if keys[key]:
        keys[key] = False
        long_beep()
    else:
        keys[key] = True
        short_beep()
    render()

def run():
    sleep_time = 1.0 / poll_frequency
    while(True):
        time.sleep(sleep_time)
        result = lc.command_str("4:key")
        args = result.split(",")
        if len(args) < 2:
            continue
        key = int(args[0])
        if key:
           count = int(args[1])
           if count <= too_short_press:
               continue
           on_key(key, True if count >= long_press else False)

def run_test():
    sleep_time = 1.0 / poll_frequency
    while(True):
        time.sleep(sleep_time)
        result = lc.command_str("4:key")
        args = result.split(",")
        if len(args) < 2:
            continue
        key = int(args[0])
        count = int(args[1])
        if key:
          if count <= too_short_press:
              print "debounce"
              continue
          press = ""
          if count >= long_press:
              lc.command(str(long_press_tone) + "," + str(long_press_duration) + ":ton")
              press = "long"
          else:
              lc.command(str(short_press_tone) + "," + str(short_press_duration) + ":ton")
              press = "short"
          print "Key: " + str(key) + " " + press + " press"

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


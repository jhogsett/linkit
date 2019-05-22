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

num_sends = 5
send_delay = 0.1

def begin():
    lc.begin() #verbose_mode)
    mc.begin("remote", verbose_mode, None, None, None, num_sends, send_delay)
    send_local("3,-1,-1:key")
    kb.begin(key_callback, verbose_mode)
    lc.stop_all()
    lc.command(":::pau")
    lc.command("1:cnt")
    lc.command("4:cnt:ton")

def run():
    while True:
        kb.poll_once()

def key_callback(key, long_press):
    button_set = settings.long_press if long_press else settings.short_press
    button = button_set[key-1]
    ui.report_verbose_alt(str(button))

    if "macro" in button:
        do_macro(button["macro"])
        if not long_press:
            kb.long_beep()

    if "color" in button:
        do_color(button["color"])
        if not long_press:
            kb.alt_beep()

    if "random" in button:
        do_random(button["random"])
        if not long_press:
            kb.alt_beep()

    if "cmd" in button:
        do_cmd(button["cmd"])
        if not long_press:
            kb.short_beep()

    if "raw" in button:
        do_raw(button["raw"])
        if not long_press:
            kb.long_beep()

    if "effect" in button:
        do_effect(button["effect"])

# ----------------------------------------

def broadcast(cmd):
    mc.broadcast(cmd)
    ui.report_verbose_alt("broadcast: " + cmd)

def send_local(cmd):
    command = "::1:pau:" + cmd + ":1:cnt"
    lc.command(command)
    ui.report_verbose_alt2("local: " + command)

def do_cmd(cmd):
    command = "3:pau:" + cmd + ":3:cnt"
    broadcast(command)

def do_color(color):
    command = "2:pau:" + color + ":flo:1:cnt:flu"
    broadcast(command)

def do_random(random):
    command = "2:pau:" + random + ":1:cnt:flu"
    broadcast(command)

def do_macro(macro):
    command = "1:pau:2:cnt:" + str(macro) + ":run"
    broadcast(command)

def do_raw(stop):
    command = stop
    broadcast(command)

def do_effect(effect):
    send_local(effect)

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


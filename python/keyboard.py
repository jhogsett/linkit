#!/usr/bin/python

import time

import app_ui as ui
import utils 
import led_command as lc
import tones as t

global poll_frequency, long_press, too_short_press
global sleep_time, key_callback, verbose_mode

verbose_mode = False
poll_frequency = 20
long_press = 50
too_short_press = 3
sleep_time = 1.0 / poll_frequency

def begin(callback=None, verbose_mode_=False):
    global key_callback, verbose_mode

    if callback == None:
        callback = default_callback
    key_callback = callback

    verbose_mode = verbose_mode_   

    lc.begin() #verbose_mode)
    lc.stop_all()

    t.begin(verbose_mode)

    enable_keyboard()
    t.store_long_press_tone()

def default_callback(key, long_press):
    print "key: {} long press: {}".format(key, long_press)
    t.activate2() if long_press else t.activate()

def poll_forever():
    while(True):
        poll_once()

def poll_wait():
    time.sleep(sleep_time)

def poll_once():
    poll()
    poll_wait()

def poll():
    result = lc.command_str("4:key")
    args = result.split(",")
    if len(args) < 2:
        return
    key = int(args[0])
    if key:
       count = int(args[1])
       if count <= too_short_press:
           return
       key_callback(key, True if count >= long_press else False)

# ========================================

def send(command):
    lc.command("::3:pau:" + command + ":3:cnt:1:cnt")
    ui.report_verbose_alt("sent: " + command)

def enable_keyboard():
    send(":::pau")
    send("4,0:cfg:1:cnt:4:cnt")



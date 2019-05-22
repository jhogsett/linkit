#!/usr/bin/python

import time

import app_ui as ui
import utils 
import led_command as lc

global poll_frequency, long_press, too_short_press, short_press_tone, short_press_duration, alt_press_tone, alt_press_duration, long_press_tone, long_press_duration
global sleep_time, key_callback, verbose_mode

verbose_mode = False
poll_frequency = 20
long_press = 50
too_short_press = 3
short_press_tone = 2800
short_press_duration = 50
alt_press_tone = 2200
alt_press_duration = 50
long_press_tone = 3100
long_press_duration = 200
sleep_time = 1.0 / poll_frequency
key_callback = None

def begin(callback=None, verbose_mode_=False):
    global key_callback, verbose_mode

    if callback == None:
        callback = default_callback
    key_callback = callback

    verbose_mode = verbose_mode_   

    lc.begin(verbose_mode)
    lc.stop_all()

    enable_keyboard()
    store_long_beep_on_device()

def default_callback(key, long_press):
    print "key: {} long press: {}".format(key, long_press)
    long_beep() if long_press else short_beep()

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

def enable_keyboard():
    lc.command("4,0:cfg:1:cnt:3:cnt")

def store_long_beep_on_device():
    lc.command_str("3,-1,0:key:0:set:" + str(long_press_tone) + "," + str(long_press_duration) + ":ton")

def short_beep():
    lc.command(str(short_press_tone) + "," + str(short_press_duration) + ":ton")

def alt_beep():
    lc.command(str(alt_press_tone) + "," + str(alt_press_duration) + ":ton")

def long_beep():
    lc.command(str(long_press_tone) + "," + str(long_press_duration) + ":ton")


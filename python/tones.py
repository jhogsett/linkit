#!/usr/bin/python

import time

import app_ui as ui
#import utils 
import led_command as lc

tones = [309, 412, 583,  734, 778, 824,  1166, 1235, 1309, 1387,  1962, 2078, 2202]

tone_very_low1 = 0
tone_very_low2 = 1
tone_very_low3 = 2
tone_very_low = 1

tone_low4 = 3
tone_low5 = 4
tone_low6 = 5
tone_low = 4

tone_mid1 = 6
tone_mid2 = 7
tone_mid3 = 8
tone_mid4 = 9
tone_mid = 6

# mid/low good on/off toggle

tone_high1 = 10
tone_high2 = 11
tone_high1 = 12
tone_high = 11

very_short = 50
short = 100
long = 300
very_long = 1000

global sleep_time, key_callback, verbose_mode
verbose_mode = False

def begin(verbose_mode_=False):
    global verbose_mode
    verbose_mode = verbose_mode_   
    lc.begin() #verbose_mode)
    lc.stop_all()

# ========================================

def send(command):
    lc.command("::3:pau:" + command + ":3:cnt:1:cnt")
    ui.report_verbose_alt("sent: " + command)

def tone(note, duration):
    freq = tones[note]
    send(str(freq) + "," + str(duration) + ":ton")

def multi_tone(times, note, duration):
    for n in range(times):
        tone(note, duration)
        time.sleep(1000.0 / duration)

def store_long_press_tone(note=None, duration=None):
    if note == None:
        note = tone_high
    if duration == None:
        duration = short
    freq = tones[note]

    # this causes two beeps on first key press
    #send("3,-1,0:key:0:set:" + str(freq) + "," + str(duration) + ":ton")
    lc.command_str("3,-1,0:key:0:set:" + str(freq) + "," + str(duration) + ":ton")

# functional tone types

def hello():
    tone(tone_very_low, very_short)
    tone(tone_low, very_short)
    tone(tone_mid, very_short)
    tone(tone_high, very_short)

def goodbye():
    tone(tone_high, very_short)
    tone(tone_mid, very_short)
    tone(tone_low, very_short)
    tone(tone_very_low, very_short)

def toggle_on():
    tone(tone_mid, short)

def toggle_off():
    tone(tone_low, short)

def gone():
    tone(tone_very_low, long)

def keypress():
    tone(tone_high, very_short)

def activate():
    tone(tone_high, short)

def activate2():
    tone(tone_high, very_short)
    time.sleep(short / 1000.0)
    tone(tone_high, very_short)

def long_activate():
    tone(tone_high, long)

def right():
    tone(tone_low, very_short)
    tone(tone_high, very_short)

def wrong():
    tone(tone_very_low, very_long)


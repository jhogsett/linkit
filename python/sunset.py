#!/usr/bin/python

import wx
import sys
import time
from subprocess import call
import argparse

global app_description, verbose_mode, api_key, zipcode, timezone_offset, sleep_time, last_command_time, command_line
app_description = None
verbose_mode = None
api_key = None
zipcode = None
update_frequency = None
timezone_offset = None
last_command_time = None
command_line = None

def get_options():
    global app_description, verbose_mode, api_key, zipcode, update_freq, timezone_offset, command_line
    app_description = "Sunset Commander - Apollo Lighting System v.0.0 9-0-18"

    parser = argparse.ArgumentParser(description=app_description)
    parser.add_argument("-v", "--verbose", dest="verbose", action="store_true", help="display verbose info (False)")
    parser.add_argument("-k", "--apikey", dest="apikey", help="openweathermap.org api key")
    parser.add_argument("-z", "--zipcode", dest="zipcode", help="local postal zip code")
    parser.add_argument("-f", "--update_frequency", type=int, dest="update_freq", default=15, help="update freqency (seconds) (15)")
    parser.add_argument("-t", "--timezone_offset", type=int, dest="timezone_offset", default=8, help="timezone offset from UTC (hours) (8)")
    parser.add_argument("command_line", nargs=argparse.REMAINDER, help="command line to run at sunset (None)")
    args = parser.parse_args()
    verbose_mode = args.verbose
    api_key = args.apikey
    zipcode = args.zipcode
    update_freq = args.update_freq
    timezone_offset = args.timezone_offset
    command_line = args.command_line

def validate_options():
    global command_line
    if api_key == None:
        print "api_key must be specified"
        sys.exit()
    if zipcode == None:
        print "zipcode must be specified"
        sys.exit()
    if len(command_line) == 0:
	print "command_line must be specified"
	sys.exit()
    command_line = " ".join(command_line)

def setup():
    get_options()
    validate_options()
    wx.begin(api_key, zipcode, timezone_offset)

def do_command():
      call(command_line, shell=True)

def loop():
    global last_command_time
    try:
        data = wx.get_daily_data()
        current_time = int(wx.current_time())
        sunset_time = int(wx.daily_sunset(data))

#        sunset_time = current_time

        print "Sunset: " + wx.format_unix_timestamp(sunset_time)
        print "Current time: " + wx.format_unix_timestamp(current_time)

        if last_command_time == None:
	    # command never sent, record an earlier time for comparison
            print "command not yet sent"
	    last_command_time = current_time
            return

	if last_command_time == sunset_time:
	    # command already sent
            print "command already sent"
	    return

	if sunset_time > current_time:
	    # not time to send command yet
	    print "waiting for sunset ..."
	    return

	# sunset_time is in the past but doesn't match
        # the last time the command was sent, so sent it
        print "doing command ..."
        do_command()

	# record that it was sent
        last_command_time = sunset_time

    finally:
	print
        time.sleep(update_freq)

if __name__ == '__main__':
    setup()
    while True:
        try:
            loop()
        except KeyboardInterrupt:
            sys.exit("\nExiting...\n")
        except Exception:
            raise


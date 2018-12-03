#!/usr/bin/python

import wx
import sys
import time
from subprocess import call
import argparse

global app_description, verbose_mode, api_key, zipcode, update_freq, timezone_offset, sleep_time, last_command_time, dryrun_mode, command_line, dryrun_time, command_sent, target_time, dryrun_time
app_description = None
verbose_mode = None
api_key = None
zipcode = None
update_freq = None
timezone_offset = None
last_command_time = None
command_line = None
dryrun_mode = None
dryrun_time = None
command_sent = True
target_time = None
dryrun_time = None

def get_options():
    global app_description, verbose_mode, api_key, zipcode, update_freq, timezone_offset, dryrun_mode, command_line
    app_description = "Sunrise Commander - Apollo Lighting System v.0.0 9-0-18"

    parser = argparse.ArgumentParser(description=app_description)
    parser.add_argument("-v", "--verbose", dest="verbose", action="store_true", help="display verbose info (False)")
    parser.add_argument("-k", "--apikey", dest="apikey", help="openweathermap.org api key")
    parser.add_argument("-z", "--zipcode", dest="zipcode", help="local postal zip code")
    parser.add_argument("-f", "--update_frequency", type=int, dest="update_freq", default=15, help="update freqency (seconds) (15)")
    parser.add_argument("-t", "--timezone_offset", type=int, dest="timezone_offset", default=8, help="timezone offset from UTC (hours) (8)")
    parser.add_argument("-r", "--dryrun", dest="dryrun", action="store_true", help="use a fake event time (False)")
    parser.add_argument("command_line", nargs=argparse.REMAINDER, help="command line to run at sunrise (None)")
    args = parser.parse_args()
    verbose_mode = args.verbose
    api_key = args.apikey
    zipcode = args.zipcode
    update_freq = args.update_freq
    timezone_offset = args.timezone_offset
    dryrun_mode = args.dryrun
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
    if verbose_mode:
        print command_line
    call(command_line, shell=True)


#at each check:
#  sunrise may be in the future or the past
#  current time may be before or after
#
#when a new sunrise time is seen, it becomes the new target
#
#when the target changes, reset "sent" flag because a new one needs to be sent
#
#if the current time is > the target, send the command and set the "sent" flag

def set_dryrun_time():
    global dryrun_time
    data = wx.get_daily_data()
    current_time = int(wx.current_time())
    dryrun_time = current_time + (3 * update_freq)
    print "dry run: setting sunset time to " + wx.format_unix_timestamp(dryrun_time)

def loop():
    global last_command_time, target_time, dryrun_time, command_sent
    try:
        data = wx.get_daily_data()
        current_time = int(wx.current_time())
        event_time = int(wx.daily_sunrise(data))

	if dryrun_mode:
	    if dryrun_time == None:
                set_dryrun_time()
            event_time = dryrun_time

        print "Sunrise: " + wx.format_unix_timestamp(event_time)
        print "Current time: " + wx.format_unix_timestamp(current_time)

        if target_time != None and current_time > target_time:
            print "target time is in the past"
	    if not command_sent:
                print "sending command"
                do_command()
                command_sent = True
		if dryrun_mode:
                     set_dryrun_time()
	if target_time == None or target_time < event_time:
	    print "target time not set or in the past"
            if event_time <= current_time:
                print "sunrise time still in the past"
            if event_time > current_time:
	        target_time = event_time
                print "new target time: " + wx.format_unix_timestamp(target_time)
                command_sent = False
        print

    except requests.exceptions.ConnectionError as e:
        if verbose_mode:
            print("ignoring error: ", str(e))

    except KeyError as e:
        if verbose_mode:
            print("ignoring error: ", str(e))
    finally:
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


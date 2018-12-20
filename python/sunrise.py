#!/usr/bin/python

import wx
import sys
import time
from subprocess import call
import argparse
from requests.exceptions import ConnectionError
import terminal_colors as tc
import app_ui as ui

global app_description, verbose_mode, api_key, zipcode, update_freq, timezone_offset, sleep_time, last_command_time, dryrun_mode, command_line, dryrun_time, command_sent, target_time, dryrun_time, target_times
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
target_times = []

def get_options():
    global app_description, verbose_mode, api_key, zipcode, update_freq, timezone_offset, dryrun_mode, command_line, event_type, trigger_offset
    app_description = "Sunrise/Sunset Commander - Apollo Lighting System v.1.0 12-19-18"

    parser = argparse.ArgumentParser(description=app_description)
    parser.add_argument("-v", "--verbose", dest="verbose", action="store_true", help="display verbose info (False)")
    parser.add_argument("-k", "--apikey", dest="apikey", help="openweathermap.org api key")
    parser.add_argument("-z", "--zipcode", dest="zipcode", help="local postal zip code")
    parser.add_argument("-f", "--update_frequency", type=int, dest="update_freq", default=15, help="update freqency (seconds) (15)")
    parser.add_argument("-t", "--timezone_offset", type=int, dest="timezone_offset", default=8, help="timezone offset from UTC (hours) (8)")
    parser.add_argument("-r", "--dryrun", dest="dryrun", action="store_true", help="use a fake event time (False)")
    parser.add_argument("-e", "--event_type", dest="event_type", default="sunrise", help="triggering event 'sunset' or 'sunrise' (sunrise)")
    parser.add_argument("-o", "--trigger_offset", type=int, dest="trigger_offset", default=0, help="+/- minutes offset for triggering (0)")
    parser.add_argument("command_line", nargs=argparse.REMAINDER, help="command line to run at sunrise (None)")
    args = parser.parse_args()
    verbose_mode = args.verbose
    api_key = args.apikey
    zipcode = args.zipcode
    update_freq = args.update_freq
    timezone_offset = args.timezone_offset
    dryrun_mode = args.dryrun
    event_type = args.event_type
    trigger_offset = args.trigger_offset
    command_line = args.command_line

def validate_options():
    global command_line
    if api_key == None:
        sys.exit("\napi_key must be specified\n")
    if zipcode == None:
        sys.exit("\nzipcode must be specified\n")
    if len(command_line) == 0:
	sys.exit("\ncommand_line must be specified\n")
    if not event_type in ["sunrise", "sunset"]:
        sys.exit("\nevent type must be one of: sunrise, sunset\n")
    command_line = " ".join(command_line)

def introduction():
    ui.app_description(app_description)

    ui.report_verbose("verbose mode")
    ui.report_verbose("zip code: " + zipcode)
    ui.report_verbose("update frequency: " + str(update_freq) + " seconds")
    ui.report_verbose("timezone offset: " + str(timezone_offset) + " hours")
    ui.report_verbose("dry run: " + str(dryrun_mode))
    ui.report_verbose("event type: " + event_type)
    ui.report_verbose("trigger offset: " + str(trigger_offset) + " minutes")
    ui.report_verbose("command line: " + command_line)
    ui.report_verbose()

def initialize():
    get_options()
    validate_options()
    wx.begin(api_key, zipcode, timezone_offset)
    ui.begin(verbose_mode)
    introduction()

def do_command():
    ui.report_verbose(command_line)
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
    ui.report_warn("dry run: setting sunset time to " + wx.format_unix_timestamp(dryrun_time))

def loop():
    global last_command_time, target_time, dryrun_time, command_sent, target_times
    try:
        data = wx.get_daily_data()
        current_time = int(wx.current_time())

        if event_type == "sunrise":
            event_time = int(wx.daily_sunrise(data))
        else:
            event_time = int(wx.daily_sunset(data))
        event_time += trigger_offset * 60

        if dryrun_mode:
            if dryrun_time == None:
                set_dryrun_time()
            event_time = dryrun_time

        if event_time not in target_times:
            if event_time > current_time:
                target_times.append(event_time)
                ui.report_info_alt("New target time added to list: " + wx.format_unix_timestamp(event_time))
            else:
                ui.report_error("New target time is in the past: " + wx.format_unix_timestamp(event_time))

        ui.report_info("Current time: " + wx.format_unix_timestamp(current_time))

        for target_time in target_times:
            ui.report_info(event_type.capitalize() + " time: " + wx.format_unix_timestamp(target_time))

        new_target_times = []
        command_sent = False
        send_trigger_time = 0
        margin_time = 60
        for target_time in target_times:
            if current_time >= target_time:
                # target time has been reached

                # see if this event is close to a triggering event this round
                if send_trigger_time != 0:

                    # this target time must exceed the triggering time 
                    # by margin seconds to not be skipped
                    if target_time < send_trigger_time + margin_time:
                        # this target time is within the margin seconds 
                        # of the triggering event so skip it
                        ui.report_info("skipping near duplicate event")
                        continue
                if command_sent == False:
                    ui.report_info("sending command")
                    do_command()
                    command_sent = True
                    send_trigger_time = target_time
                    if dryrun_mode:
                        set_dryrun_time()
            else:
                new_target_times.append(target_time)
        target_times = new_target_times
        print

    except ConnectionError as e:
        ui.report_verbose("ignoring error: " + str(e))

    except KeyError as e:
        ui.report_verbose("ignoring error: " + str(e))
    finally:
        time.sleep(update_freq)

if __name__ == '__main__':
    initialize()
    while True:
        try:
            loop()
        except KeyboardInterrupt:
            sys.exit("\nExiting...\n")
        except Exception:
            raise


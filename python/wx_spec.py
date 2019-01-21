#!/usr/bin/python

import sys
import argparse
import terminal_colors as tc
import app_ui as ui
import os
import json

#import time
#import datetime

import wx

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
    introduction()

def reset_library():
    wx.begin(api_key, zip_code, timezone_offset, verbose_mode, quiet_mode)

def fixture_path(filename):
    script_path = os.getcwd()
    return os.path.join(script_path, "spec_fixtures", filename)

def load_fixture(filename):
    filename = fixture_path(filename)
    ui.report_verbose("opening fixture file: " + filename)
    with open(filename, "r") as text_file:
        data_str = text_file.read()
    return data_str

def load_fixture_json(filename):
    json_str = load_fixture(filename)
    json_data = json.loads(json_str)
    ui.report_verbose("parsed json data: " + str(json_data))
    return json_data

def expected(text):
    return tc.green(text)

def got(text):
    return tc.red(text)

def report_failed_expectation(description, message):
    ui.report_error("failed expectation: " + description);
    ui.report_info_alt(message)

def expect_equal(description, expected_, got_):
    if expected_ == got_:
        return True
    else:
        message = "expected: " + expected(str(expected_)) + " got: " + got(str(got_))
        report_failed_expectation(description, message)

def loop():

    ui.report_info("known good daily data")
    json_data = load_fixture_json("weather_daily_known_good.json")
    expect_equal("daily timestamp", wx.daily_timestamp(json_data), 1548007800)
    expect_equal("daily visibility", wx.daily_visibility(json_data), 16093)
    expect_equal("daily sunrise", wx.daily_sunrise(json_data), 1547997595)
    expect_equal("daily sunset", wx.daily_sunset(json_data), 1548033483)
    expect_equal("daily wind speed", str(wx.daily_wind_speed(json_data)), "14.99")
    expect_equal("daily wind direction", wx.daily_wind_deg(json_data), 190)
    expect_equal("daily pressure", wx.daily_pressure(json_data), 1018)
    expect_equal("daily humidity", wx.daily_humidity(json_data), 93)
    expect_equal("daily temp min", str(wx.daily_temp_min(json_data)), "55.04")
    expect_equal("daily temp max", str(wx.daily_temp_max(json_data)), "60.8")
    expect_equal("daily temp", str(wx.daily_temp(json_data)), "57.63")
    expect_equal("daily description", str(wx.daily_description(json_data)), "light rain")
    expect_equal("daily conditions", str(wx.daily_conditions(json_data)), "Rain")
    expect_equal("daily conditions id", wx.daily_id(json_data), 500)

    ui.report_info("empty daily data")
    json_data = load_fixture_json("weather_daily_empty.json")
    expect_equal("daily timestamp", wx.daily_timestamp(json_data), 0)
    expect_equal("daily visibility", wx.daily_visibility(json_data), 0)
    expect_equal("daily sunrise", wx.daily_sunrise(json_data), 0)
    expect_equal("daily sunset", wx.daily_sunset(json_data), 0)
    expect_equal("daily wind speed", wx.daily_wind_speed(json_data), 0.0)
    expect_equal("daily wind direction", wx.daily_wind_deg(json_data), 0)
    expect_equal("daily pressure", wx.daily_pressure(json_data), 0)
    expect_equal("daily humidity", wx.daily_humidity(json_data), 0)
    expect_equal("daily temp min", wx.daily_temp_min(json_data), 0.0)
    expect_equal("daily temp max", wx.daily_temp_max(json_data), 0.0)
    expect_equal("daily temp", wx.daily_temp(json_data), 0.0)
    expect_equal("daily description", str(wx.daily_description(json_data)), "")
    expect_equal("daily conditions", str(wx.daily_conditions(json_data)), "")
    expect_equal("daily conditions id", wx.daily_id(json_data), 0)


    sys.exit()

if __name__ == '__main__':
    initialize()
    while True:
        try:
            loop()
        except KeyboardInterrupt:
            sys.exit("\nExiting...\n")
        except Exception:
            raise


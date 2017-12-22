#!/usr/bin/python

import serial
import time
import requests
import json
import sys
#import logging
import os
#import traceback
import sys
import terminal_colors as tc
import argparse
import datetime

#script_path = os.getcwd()
#log_path = script_path + "/circleci.log"
#print "logging to " + log_path

#logging.basicConfig(filename=log_path, level=logging.INFO, format='%(asctime)s %(message)s')
#logging.info("Circleci7.py started")

global app_description, verbose_mode, api_key, zip_code, update_freq, retry_delay
app_description = None
verbose_mode = None
api_key = None
zip_code = None
update_freq = None
retry_delay = None

def initialize():
    global app_description
    app_description = "LED Weather Forecaster v.0.0 12-21-2017"

def get_options():
    global verbose_mode, api_key, zip_code, update_freq, retry_delay
    parser = argparse.ArgumentParser(description=app_description)
    parser.add_argument("-v", "--verbose", dest="verbose", action='store_true', help='display verbose info (False)')
    parser.add_argument("-k", "--apikey", dest="apikey", help="openweathermap.org api key")
    parser.add_argument("-z", "--zipcode", dest="zipcode", help="local postal zip code")
    parser.add_argument("-f", "--update_frequency", type=int, dest="update_freq", default=900, help="update freqency (seconds) (900)")
    parser.add_argument("-d", "--retry_delay", type=int, dest="retry_delay", default=5, help="connection retry delay (seconds) (5)")
    args = parser.parse_args()
    verbose_mode = args.verbose
    api_key = args.apikey
    zip_code = args.zipcode
    update_freq = args.update_freq
    retry_delay = args.retry_delay

def report_error(message):
    print tc.red(message)

def report_info(message):
    print tc.white(message)

def report_verbose(message):
    if verbose_mode:
        print tc.yellow(message)

def report_json(json):
    print tc.green(str(json))

# returns True if they're valid
def validate_options():
    errors = False
    if api_key == None:
        errors = True
        report_error("api_key must be provided")
    if zip_code == None:
        errors = True
        report_error("zip_code must be provided")
    if update_freq < 1:
        errors = True
        report_error("update frequency must be positive")
    return not errors

def introduction():
    print tc.magenta("\n" + app_description + "\n")
    report_verbose("verbose mode")
    report_verbose("api key: %s" % api_key)
    report_verbose("retry delay: %s" % str(retry_delay))
    report_info("zip code: %s" % zip_code)
    report_info("update frequency (seconds): %s" % str(update_freq))
    print

def get_daily_url():
    return "http://api.openweathermap.org/data/2.5/weather?zip=%s&APPID=%s&units=imperial" % (zip_code, api_key)

def get_forecast_url():
    return "http://api.openweathermap.org/data/2.5/forecast?zip=%s&APPID=%s&units=imperial" % (zip_code, api_key)

def retrieve_data(url):
    request = requests.get(url)
    request = request.text.encode('utf-8')
    json_data = json.loads(request)
    return json_data

def daily_main(data):
    return data["main"]

def daily_sys(data):
    return data["sys"]

def daily_wind(data):
    return data["wind"]

def daily_timestamp(data):
    return data["dt"]

def daily_visibility(data):
    return data["visibility"]

def daily_sunrise(data):
    return daily_sys(data)["sunrise"]

def daily_sunset(data):
    return daily_sys(data)["sunset"]

def daily_wind_speed(data):
    return daily_wind(data)["speed"]

def daily_wind_deg(data):
    return daily_wind(data)["deg"]

def daily_pressure(data):
    return daily_main(data)["pressure"]

def daily_humidity(data):
    return daily_main(data)["humidity"]

def daily_temo_min(data):
    return daily_main(data)["temp_min"]

def daily_temp_max(data):
    return daily_main(data)["temp_max"]

def daily_temp(data):
    return daily_main(data)["temp"]

def format_unix_timestamp(ts):
    return format_datetime(datetime.datetime.fromtimestamp(ts))

def format_datetime(dt):
    return dt.strftime('%Y-%m-%d %H:%M:%S')

def report_weather(data):
    print tc.green("Timestamp: ") + tc.cyan(format_unix_timestamp(int(daily_timestamp(data))))



def setup():
    initialize()
    get_options()
    if not validate_options():
        sys.exit("\nExiting...\n")
    introduction()

def loop():
    url = get_daily_url()
    report_verbose("requesting: %s" % url)
    data = retrieve_data(url)
    report_verbose("received data:")
    if verbose_mode:
        report_json(data)
    report_weather(data)

if __name__ == '__main__':
    setup()
    while True:
        try:
            loop()
            report_verbose("pausing until next update")
            time.sleep(update_freq)
        except requests.exceptions.ConnectionError:
            report_error("connection error - retrying")
            time.sleep(retry_delay)
        except KeyboardInterrupt:
            sys.exit("\nExiting...\n")
        except Exception:
            raise


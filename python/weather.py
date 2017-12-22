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
    if verbose_mode:
        print tc.yellow("verbose mode")
        print tc.cyan("api key: ") + tc.green(api_key)
        print tc.cyan("retry delay: ") + tc.green(str(retry_delay))
    print tc.cyan("zip code: ") + tc.green(zip_code)
    print tc.cyan("update frequency (seconds): ") + tc.green(str(update_freq))
    print

def get_daily_url():
    return "http://api.openweathermap.org/data/2.5/weather?zip=%s&APPID=%s&units=imperial" % (zip_code, api_key)

def get_forecast_url():
    return "http://api.openweathermap.org/data/2.5/forecast?zip=%s&APPID=%s&units=imperial" % (zip_code, api_key)

def retrieve_data():
    request = requests.get(get_url())
    request = request.text.encode('utf-8')
    json_data = json.loads(request)
    return json_data

def setup():
    initialize()
    get_options()
    if not validate_options():
        sys.exit("\nExiting...\n")
    introduction()

def loop():
    print "do something"


if __name__ == '__main__':
    setup()
    while True:
        try:
            loop()
            if verbose_mode:
                print tc.yellow("pausing until next update")
            time.sleep(update_freq)
        except requests.exceptions.ConnectionError:
            print tc.red("connection error - retrying")
            time.sleep(retry_delay)
        except KeyboardInterrupt:
            sys.exit("\nExiting...\n")
        except Exception:
            raise


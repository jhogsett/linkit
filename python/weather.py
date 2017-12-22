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

global app_description, verbose_mode, api_key, zip_code, update_freq
app_description = None
verbose_mode = None
api_key = None
zip_code = None
update_freq = None

def initialize():
    global app_description
    app_description = "LED Weather Forecaster v.0.0 12-21-2017"

def get_options():
    global verbose_mode, api_key, zip_code, update_freq
    parser = argparse.ArgumentParser(description=app_description)
    parser.add_argument("-v", "--verbose", dest="verbose", action='store_true', help='display verbose info (False)')
    parser.add_argument("-k", "--apikey", dest="apikey", help="openweathermap.org api key")
    parser.add_argument("-z", "--zipcode", dest="zipcode", help="local postal zip code")
    parser.add_argument("-f", "--update_frequency", type=int, dest="update_freq", default=900, help="update freqency (seconds) (900)")
    args = parser.parse_args()
    verbose_mode = args.verbose
    api_key = args.apikey
    zip_code = args.zipcode
    update_freq = args.update_freq

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
    print tc.cyan("zip code: ") + tc.green(zip_code)
    print tc.cyan("update frequency (seconds): ") + tc.green(str(update_freq))

    print











if __name__ == '__main__':
    initialize()
    get_options()
    if not validate_options():
        sys.exit("\nExiting...\n")
    introduction()


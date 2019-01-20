#!/usr/bin/python

import time
import requests
import json
import sys
#import logging
import os
#import traceback
import argparse
import datetime
import weather_conditions as wc
import app_ui as ui

#script_path = os.getcwd()
#log_path = script_path + "/circleci.log"
#print "logging to " + log_path

#logging.basicConfig(filename=log_path, level=logging.INFO, format='%(asctime)s %(message)s')
#logging.info("Circleci7.py started")

global verbose_mode, api_key, zip_code, timezone_offset, min_api_delay, quiet_mode
verbose_mode = None
quiet_mode = None
api_key = None
zip_code = None
timezone_offset = None
min_api_delay = 2

## --------------------------------------------------------------------------
## Public API
## --------------------------------------------------------------------------

## general

def begin(api_key_, zip_code_, timezone_offset_, verbose_mode_, quiet_mode_):
    global api_key, zip_code, timezone_offset, verbose_mode, quiet_mode
    api_key = api_key_
    zip_code = zip_code_
    timezone_offset = timezone_offset_
    verbose_mode = verbose_mode_
    quiet_mode = quiet_mode_
    ui.begin(verbose_mode, quiet_mode)

def current_time():
    dt = datetime.datetime.now()
    return int(time.mktime(dt.timetuple()))

def get_datetime(ts):
    ts = timezone_timestamp(ts)
    return datetime.datetime.fromtimestamp(ts)

def current_weekday():
    dt = get_datetime(current_time())
    return dt.weekday()


## formatting

def format_unix_timestamp(ts):
    return format_datetime(get_datetime(ts))

def format_datetime(dt):
    return dt.strftime('%A %Y-%m-%d %I:%M:%S %p')


## daily

def get_daily_data():
    url = get_daily_url()
    ui.report_verbose("requesting: %s" % url)
    data = retrieve_data(url)
    ui.report_verbose("received data:")
    ui.report_verbose_alt(str(data))
    return data

def daily_sunrise(data):
    return daily_sys(data)["sunrise"]

def daily_sunset(data):
    return daily_sys(data)["sunset"]

def daily_city(data):
    return data["name"]

def daily_lat(data):
    return daily_coord(data)["lat"]

def daily_lon(data):
    return daily_coord(data)["lon"]

def daily_timestamp(data):
    return data["dt"]

def daily_visibility(data):
    try:
        return data["visibility"]
    except:
        return 5280 * 10

def daily_wind_speed(data):
    return daily_wind(data)["speed"]

def daily_wind_deg(data):
    return daily_wind(data)["deg"]

def daily_pressure(data):
    return daily_main(data)["pressure"]

def daily_humidity(data):
    return daily_main(data)["humidity"]

def daily_temp_min(data):
    return daily_main(data)["temp_min"]

def daily_temp_max(data):
    return daily_main(data)["temp_max"]

def daily_temp(data):
    return daily_main(data)["temp"]

def daily_description(data):
    return daily_weather(data)["description"]

def daily_conditions(data):
    return daily_weather(data)["main"]

def daily_id(data):
    return daily_weather(data)["id"]


## forecast

def get_forecast_data():
    url = get_forecast_url()
    ui.report_verbose("requesting: %s" % url)
    data = retrieve_data(url)
    ui.report_verbose("received data:")
    ui.report_verbose_alt(str(data))
    return data

def forecast_count(data):
    return len(forecast_list(data))

def forecast_timestamp(data, entry):
    return forecast_entry(data, entry)["dt"]

def forecast_pressure(data, entry):
    return forecast_main(data, entry)["pressure"]

def forecast_temp(data, entry):
    return forecast_main(data, entry)["temp"]

def forecast_temp_min(data, entry):
    return forecast_main(data, entry)["temp_min"]

def forecast_temp_max(data, entry):
    return forecast_main(data, entry)["temp_max"]

def forecast_humidity(data, entry):
    return forecast_main(data, entry)["humidity"]

def forecast_wind_speed(data, entry):
    return forecast_wind(data, entry)["speed"]

def forecast_wind_direction(data, entry):
    return forecast_wind(data, entry)["deg"]

def forecast_cloudiness(data, entry):
    return forecast_clouds(data, entry)["all"]

def forecast_cond_id(data, entry):
    return forecast_weather(data, entry)["id"]

def forecast_conditions(data, entry):
    return forecast_weather(data, entry)["main"]

def forecast_description(data, entry):
    return forecast_weather(data, entry)["description"]

def current_weekday():
    dt = get_datetime(current_time())
    return dt.weekday()


## --------------------------------------------------------------------------
## Private
## --------------------------------------------------------------------------

## general

def retrieve_data(url):
    # returns None if there's a json loading error
    request = requests.get(url)
    request = request.text.encode('utf-8')
    json_data = None
    try:
        json_data = json.loads(request)
    except ValueError as e:
        ui.report_error("Exception caught loading JSON data: " + str(e))
    ui.report_verbose("api delay...")
    time.sleep(min_api_delay)        
    return json_data

def timezone_timestamp(ts):
    return int(ts) - (timezone_offset * 60 * 60)

def weather_entry(key, value):
    return tc.green(key) + ": " + tc.cyan(str(value))


## daily

def get_daily_url():
    return "http://api.openweathermap.org/data/2.5/weather?zip=%s&APPID=%s&units=imperial" % (zip_code, api_key)

def daily_main(data):
    return data["main"]

def daily_sys(data):
    return data["sys"]

def daily_wind(data):
    return data["wind"]

def daily_weather(data):
    return data["weather"][0]

def daily_coord(data):
    return data["coord"]

def get_forecast_url():
    return "http://api.openweathermap.org/data/2.5/forecast?zip=%s&APPID=%s&units=imperial" % (zip_code, api_key)


## forecast

def forecast_list(data):
    return data["list"]

def forecast_entry(data, entry):
    return forecast_list(data)[int(entry)]

def forecast_main(data, entry):
    return forecast_entry(data, entry)["main"]

def forecast_wind(data, entry):
    return forecast_entry(data, entry)["wind"]

def forecast_clouds(data, entry):
    return forecast_entry(data, entry)["clouds"]

def forecast_weather(data, entry):
    return forecast_entry(data, entry)["weather"][0]


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
    sys = daily_sys(data)
    if "sunrise" in sys:
        return sys["sunrise"]
    else:
        return 0

def daily_sunset(data):
    sys = daily_sys(data)
    if "sunset" in sys:
        return sys["sunset"]
    else:
        return 0

def daily_city(data):
    return data["name"]

def daily_lat(data):
    return daily_coord(data)["lat"]

def daily_lon(data):
    return daily_coord(data)["lon"]

def daily_timestamp(data):
    if "dt" in data:
        return data["dt"]
    else:
        return 0

def daily_visibility(data):
    if "visibility" in data:
        return data["visibility"]
    else:
#        return 5280 * 10
        return 0

def daily_wind_speed(data):
    wind = daily_wind(data)
    if "speed" in wind:
        return wind["speed"]
    else:
        return 0.0

def daily_wind_deg(data):
    wind = daily_wind(data)
    if "deg" in wind:
        return wind["deg"]
    else:
        return 0

def daily_pressure(data):
    main = daily_main(data)
    if "pressure" in main:
        return main["pressure"]
    else:
        return 0

def daily_humidity(data):
    main = daily_main(data)
    if "humidity" in main:
        return main["humidity"]
    else:
        return 0

def daily_temp_min(data):
    main = daily_main(data)
    if "temp_min" in main:
        return main["temp_min"]
    else:
        return 0.0

def daily_temp_max(data):
    main = daily_main(data)
    if "temp_max" in main:
        return main["temp_max"]
    else:
        return 0.0

def daily_temp(data):
    main = daily_main(data)
    if "temp" in main:
        return main["temp"]
    else:
        return 0.0

def daily_description(data):
    weather = daily_weather(data)
    if "description" in weather:
        return weather["description"]
    else:
        return ""

def daily_conditions(data):
    weather = daily_weather(data)
    if "main" in weather:
        return weather["main"]
    else:
        return ""

def daily_id(data):
    weather = daily_weather(data)
    if "id" in weather:
        return weather["id"]
    else:
        return 0


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
    if "main" in data:
        return data["main"]
    else:
        return {}

def daily_sys(data):
    if "sys" in data:
        return data["sys"]
    else:
        return {}
 
def daily_wind(data):
    if "wind" in data:
        return data["wind"]
    else:
        return {}

def daily_weather(data):
    if "weather" in data:
        return data["weather"][0]
    else:
        return {}

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


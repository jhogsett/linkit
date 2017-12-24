#!/usr/bin/python

import serial
import time
import requests
import json
import sys
#import logging
import os
#import traceback
import terminal_colors as tc
import argparse
import datetime
import weather_conditions as wc
import led_command as lc

#script_path = os.getcwd()
#log_path = script_path + "/circleci.log"
#print "logging to " + log_path

#logging.basicConfig(filename=log_path, level=logging.INFO, format='%(asctime)s %(message)s')
#logging.info("Circleci7.py started")

global app_description, verbose_mode, api_key, zip_code, update_freq, retry_delay, timezone_offset, min_api_delay, num_leds
app_description = None
verbose_mode = None
api_key = None
zip_code = None
update_freq = None
retry_delay = None
timezone_offset = None
min_api_delay = None
num_leds = None

def get_options():
    global verbose_mode, api_key, zip_code, update_freq, retry_delay, timezone_offset, min_api_delay
    parser = argparse.ArgumentParser(description=app_description)
    parser.add_argument("-v", "--verbose", dest="verbose", action='store_true', help='display verbose info (False)')
    parser.add_argument("-k", "--apikey", dest="apikey", help="openweathermap.org api key")
    parser.add_argument("-z", "--zipcode", dest="zipcode", help="local postal zip code")
    parser.add_argument("-f", "--update_frequency", type=int, dest="update_freq", default=900, help="update freqency (seconds) (900)")
    parser.add_argument("-d", "--retry_delay", type=int, dest="retry_delay", default=5, help="connection retry delay (seconds) (5)")
    parser.add_argument("-t", "--timezone_offset", type=int, dest="timezone_offset", default=8, help="timezone offset from UTC (hours) (8)")
    parser.add_argument("-m", "--min_api_delay", type=int, dest="min_api_delay", default=1, help="minimum api call delay (seconds) (1)")
    args = parser.parse_args()
    verbose_mode = args.verbose
    api_key = args.apikey
    zip_code = args.zipcode
    update_freq = args.update_freq
    retry_delay = args.retry_delay
    timezone_offset = args.timezone_offset
    min_api_delay = args.min_api_delay

def initialize():
    global app_description, num_leds
    app_description = "LED Weather Forecaster v.0.0 12-21-2017"
    get_options()
    if not validate_options():
        sys.exit("\nExiting...\n")
    lc.begin(verbose_mode)
    num_leds = lc.get_num_leds()

def report_error(message):
    print tc.red(message)

def report_info(message):
    print tc.white(message)

def report_verbose(message=""):
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

def intro_entry(key, value):
    return tc.white(key) + ": " + tc.green(str(value))

def introduction():
    print tc.magenta("\n" + app_description + "\n")

    report_verbose("verbose mode")
    report_verbose(intro_entry("API Key", api_key))
    report_verbose(intro_entry("Retry Delay", retry_delay))
    report_verbose(intro_entry("Timezone Offset", timezone_offset))
    report_verbose(intro_entry("Minimum API Delay", min_api_delay))
    report_verbose()

    data = get_daily_data()
    report_info(intro_entry("City", daily_city(data)))
    report_info(intro_entry("Latitude", daily_lat(data)))
    report_info(intro_entry("Longitude", daily_lon(data)))
    report_info(intro_entry("Zip Code", zip_code))
    report_info(intro_entry("Update Frequency", update_freq))

def get_daily_url():
    return "http://api.openweathermap.org/data/2.5/weather?zip=%s&APPID=%s&units=imperial" % (zip_code, api_key)

def get_forecast_url():
    return "http://api.openweathermap.org/data/2.5/forecast?zip=%s&APPID=%s&units=imperial" % (zip_code, api_key)

def retrieve_data(url):
    request = requests.get(url)
    request = request.text.encode('utf-8')
    json_data = json.loads(request)
    report_verbose("api delay...")
    time.sleep(min_api_delay)
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

def daily_weather(data):
    return data["weather"][0]

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

def daily_temp_min(data):
    return daily_main(data)["temp_min"]

def daily_temp_max(data):
    return daily_main(data)["temp_max"]

def daily_temp(data):
    return daily_main(data)["temp"]

def daily_city(data):
    return data["name"]

def daily_coord(data):
    return data["coord"]

def daily_lat(data):
    return daily_coord(data)["lat"]

def daily_lon(data):
    return daily_coord(data)["lon"] 

def daily_description(data):
    return daily_weather(data)["description"]

def daily_conditions(data):
    return daily_weather(data)["main"]

def daily_id(data):
    return daily_weather(data)["id"]

def format_unix_timestamp(ts):
    ts = int(ts) - (timezone_offset * 60 * 60)
    return format_datetime(datetime.datetime.fromtimestamp(ts))

def format_datetime(dt):
    return dt.strftime('%A %Y-%m-%d %I:%M:%S %p')

def weather_entry(key, value):
    return tc.green(key) + ": " + tc.cyan(str(value))

def current_time():
    dt = datetime.datetime.now()
    return(int(time.mktime(dt.timetuple())))

def report_header():
    print "\n----------------------------------------------------"

def report_footer():
    print

def report_weather(data):
    report_header()
    print weather_entry("Local Time", format_unix_timestamp(int(current_time())))
    print weather_entry("Data As Of", format_unix_timestamp(int(daily_timestamp(data))))
    print weather_entry("Visibility", daily_visibility(data))
    print weather_entry("Sunrise", format_unix_timestamp(int(daily_sunrise(data))))
    print weather_entry("Sunset", format_unix_timestamp(int(daily_sunset(data))))
    print weather_entry("Wind Speed", daily_wind_speed(data))
    print weather_entry("Wind Direction", daily_wind_deg(data))
    print weather_entry("Pressure", daily_pressure(data))
    print weather_entry("Hunidity", daily_humidity(data))
    print weather_entry("Temp Min", daily_temp_min(data))
    print weather_entry("Temp Max", daily_temp_max(data))
    print weather_entry("Temp", daily_temp(data))
    print weather_entry("Description", daily_description(data))
    print weather_entry("Conditions", daily_conditions(data))
    print weather_entry("Cond ID", daily_id(data))
    report_footer()

def get_daily_data():
    url = get_daily_url()
    report_verbose("requesting: %s" % url)
    data = retrieve_data(url)
    report_verbose("received data:")
    if verbose_mode:
        report_json(data)    
    return data

def get_forecast_data():
    url = get_forecast_url()
    report_verbose("requesting: %s" % url)
    data = retrieve_data(url)
    report_verbose("received data:")
    if verbose_mode:
        report_json(data)
    return data

def forecast_list(data):
    return data["list"]

def forecast_count(data):
    return len(forecast_list(data))

def forecast_entry(data, entry):
    return forecast_list(data)[int(entry)]

def forecast_timestamp(data, entry):
    return forecast_entry(data, entry)["dt"]

def forecast_main(data, entry):
    return forecast_entry(data, entry)["main"]

def forecast_wind(data, entry):
    return forecast_entry(data, entry)["wind"]

def forecast_clouds(data, entry):
    return forecast_entry(data, entry)["clouds"]

def forecast_weather(data, entry):
    return forecast_entry(data, entry)["weather"][0]

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

def forecast_cloudinesss(data, entry):
    return forecast_clouds(data, entry)["all"]

def forecast_description(data, entry):
    return forecast_weather(data, entry)["description"]

def forecast_conditions(data, entry):
    return forecast_weather(data, entry)["main"]

def forecast_cond_id(data, entry):
    return forecast_weather(data, entry)["id"]

def report_forecast(data):
    for x in range(0, forecast_count(data)):
        print weather_entry("Date/Time", format_unix_timestamp(int(forecast_timestamp(data, x)))),
        print weather_entry(" Pressure", forecast_pressure(data, x)),
        print weather_entry(" Temp", forecast_temp(data, x)),
        print weather_entry(" Temp Min", forecast_temp_min(data, x)),
        print weather_entry(" Temp Max", forecast_temp_max(data, x)),
        print weather_entry(" Humidity", forecast_humidity(data, x)),
        print weather_entry(" Wind Spd", forecast_wind_speed(data, x)),
        print weather_entry(" Wind Dir", forecast_wind_direction(data, x)),
        print weather_entry(" Clouds", forecast_cloudinesss(data, x)),
        print weather_entry(" Cond ID", forecast_cond_id(data, x)),
        print weather_entry(" Conditions", forecast_conditions(data, x)),
        print weather_entry(" Description", forecast_description(data, x))        

def setup_palette():
    lc.command("1:shf")

def begin_display_sequence():
    lc.command(":::pau:pau")

def end_display_sequence():
    lc.command("cnt:cnt:flu")

def send_forecast_conditions(data):
    spacer = "blk"
    begin_display_sequence()
    count = forecast_count(data)
    cmd = "era:"
    for x in xrange(count - 1, -1, -1):
        condition = forecast_cond_id(data, x)
        style = wc.weather_conditions[int(condition)]
        cmd = cmd + style + ":" + style + ":" + spacer + ":"
        if len(cmd) > lc.max_command_buffer:
            lc.command(cmd[:-1])
            cmd = ""
    if len(cmd) > 0:
        lc.command(cmd[:-1])
    end_display_sequence()




def setup():
    initialize()
    introduction()

def loop():
    daily_data = get_daily_data()
    forecast_data = get_forecast_data()

    report_weather(daily_data)
    report_forecast(forecast_data)

    send_forecast_conditions(forecast_data)

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


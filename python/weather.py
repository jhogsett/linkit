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
import wx
import app_ui as ui

#script_path = os.getcwd()
#log_path = script_path + "/circleci.log"
#print "logging to " + log_path

#logging.basicConfig(filename=log_path, level=logging.INFO, format='%(asctime)s %(message)s')
#logging.info("Circleci7.py started")

global app_description, verbose_mode, quiet_mode, api_key, zip_code, update_freq, retry_delay, timezone_offset, min_api_delay, num_leds, suppress_spew, default_lightnes, minimum_lightness, double_size, dump_mode
app_description = None
verbose_mode = None
quiet_mode = None
api_key = None
zip_code = None
update_freq = None
retry_delay = None
timezone_offset = None
min_api_delay = None
num_leds = None
suppress_spew = None
default_lightness = None
minimum_lightness = None
double_size = False
dump_mode = None

def get_options():
    global verbose_mode, api_key, zip_code, update_freq, retry_delay, timezone_offset, min_api_delay, suppress_spew, quiet_mode, dump_mode
    parser = argparse.ArgumentParser(description=app_description)
    parser.add_argument("-v", "--verbose", dest="verbose", action="store_true", help="display verbose info (False)")
    parser.add_argument("-k", "--apikey", dest="apikey", help="openweathermap.org api key")
    parser.add_argument("-z", "--zipcode", dest="zipcode", help="local postal zip code")
    parser.add_argument("-f", "--update_frequency", type=int, dest="update_freq", default=15, help="update freqency (seconds) (15)")
    parser.add_argument("-d", "--retry_delay", type=int, dest="retry_delay", default=5, help="connection retry delay (seconds) (5)")
    parser.add_argument("-t", "--timezone_offset", type=int, dest="timezone_offset", default=8, help="timezone offset from UTC (hours) (8)")
    parser.add_argument("-m", "--min_api_delay", type=int, dest="min_api_delay", default=1, help="minimum api call delay (seconds) (1)")
    parser.add_argument("-s", "--suppress_spew", dest="suppress_spew", action="store_true", help="supress period data display (False)")
    parser.add_argument("-q", "--quiet", dest="quiet", action="store_true", help="don't use terminal colors (False)")
    parser.add_argument("-w", "--write_json", dest="dump", action="store_true", help="write json data to disk (False)")
    args = parser.parse_args()
    verbose_mode = args.verbose
    api_key = args.apikey
    zip_code = args.zipcode
    update_freq = args.update_freq
    retry_delay = args.retry_delay
    timezone_offset = args.timezone_offset
    min_api_delay = args.min_api_delay
    suppress_spew = args.suppress_spew
    quiet_mode = args.quiet
    dump_mode = args.dump

def initialize():
    global app_description, num_leds, default_lightness, minimum_lightness, double_size
    app_description = "LED Weather Forecaster v.0.0 12-21-2017"
    get_options()
    if not validate_options():
        sys.exit("\nExiting...\n")
    tc.begin(quiet_mode)
    ui.begin(verbose_mode, quiet_mode)
    wx.begin(api_key, zip_code, timezone_offset, verbose_mode, quiet_mode)
    lc.begin(verbose_mode)
    lc.command(":::")
    lc.command("pau:era:flu")
    num_leds = lc.get_num_leds()
    if num_leds > 72:
        double_size = True
    default_lightness = lc.get_default_lightness()
    minimum_lightness = lc.get_minimum_lightness()

# returns True if they're valid
def validate_options():
    errors = False
    if api_key == None:
        errors = True
        ui.report_error("api_key must be provided")
    if zip_code == None:
        errors = True
        ui.report_error("zip_code must be provided")
    if update_freq < 1:
        errors = True
        ui.report_error("update frequency must be positive")
    return not errors

def intro_entry(key, value):
    return tc.white(key) + ": " + tc.green(str(value))

def introduction():
    ui.app_description(app_description)

    ui.report_verbose("verbose mode")
    ui.report_verbose(intro_entry("API Key", api_key))
    ui.report_verbose(intro_entry("Retry Delay", retry_delay))
    ui.report_verbose(intro_entry("Timezone Offset", timezone_offset))
    ui.report_verbose(intro_entry("Minimum API Delay", min_api_delay))
    ui.report_verbose(intro_entry("Default Lightness", default_lightness))
    ui.report_verbose()

    data = wx.get_daily_data()
    ui.report_info(intro_entry("City", wx.daily_city(data)))
    ui.report_info(intro_entry("Latitude", wx.daily_lat(data)))
    ui.report_info(intro_entry("Longitude", wx.daily_lon(data)))
    ui.report_info(intro_entry("Zip Code", zip_code))
    ui.report_info(intro_entry("Update Frequency", update_freq))
    ui.report_info(intro_entry("Number of LEDs", num_leds))

def report_header():
    print "\n----------------------------------------------------"

def report_footer():
    print

def weather_entry(key, value):
    return tc.green(key) + ": " + tc.cyan(str(value))

#def format_unix_timestamp(ts):
#    return wx.format_datetime(wx.get_datetime(ts))

def report_weather(data):
    report_header()
    print weather_entry("Local Time", wx.format_unix_timestamp(int(wx.current_time())))
    print weather_entry("Data As Of", wx.format_unix_timestamp(int(wx.daily_timestamp(data))))
    print weather_entry("Visibility", wx.daily_visibility(data))
    print weather_entry("Sunrise", wx.format_unix_timestamp(int(wx.daily_sunrise(data))))
    print weather_entry("Sunset", wx.format_unix_timestamp(int(wx.daily_sunset(data))))
    print weather_entry("Wind Speed", wx.daily_wind_speed(data))
    print weather_entry("Wind Direction", wx.daily_wind_deg(data))
    print weather_entry("Pressure", wx.daily_pressure(data))
    print weather_entry("Hunidity", wx.daily_humidity(data))
    print weather_entry("Temp Min", wx.daily_temp_min(data))
    print weather_entry("Temp Max", wx.daily_temp_max(data))
    print weather_entry("Temp", wx.daily_temp(data))
    print weather_entry("Description", wx.daily_description(data))
    print weather_entry("Conditions", wx.daily_conditions(data))
    print weather_entry("Cond ID", wx.daily_id(data))
    report_footer()

def report_forecast(data):
    for x in range(0, wx.forecast_count(data)):
        print weather_entry("Date/Time", wx.format_unix_timestamp(int(wx.forecast_timestamp(data, x)))),
        print weather_entry(" Pressure", wx.forecast_pressure(data, x)),
        print weather_entry(" Temp", wx.forecast_temp(data, x)),
        print weather_entry(" Temp Min", wx.forecast_temp_min(data, x)),
        print weather_entry(" Temp Max", wx.forecast_temp_max(data, x)),
        print weather_entry(" Humidity", wx.forecast_humidity(data, x)),
        print weather_entry(" Wind Spd", wx.forecast_wind_speed(data, x)),
        print weather_entry(" Wind Dir", wx.forecast_wind_direction(data, x)),
        print weather_entry(" Clouds", wx.forecast_cloudiness(data, x)),
        print weather_entry(" Cond ID", wx.forecast_cond_id(data, x)),
        print weather_entry(" Conditions", wx.forecast_conditions(data, x)),
        print weather_entry(" Description", wx.forecast_description(data, x))        

def begin_display_sequence():
    lc.command("::pau")
    lc.command("::pau:-1:sch:era") 
    lc.flush_output()

def end_display_sequence():
    lc.command("cnt:flu")
    lc.flush_output()

def count_filled(array):
    count_of_filled = 0
    for x in range(len(array)):
        if array[x] != None:
            count_of_filled += 1
    return count_of_filled

# True if filler only
def is_empty(array):
    for x in range(len(array)):
        if array[x] !=1:
            return False
    return True

num_slots = 6
num_segments = 8

global condition_slots, temperature_slots, humidity_slots, wind_speed_slots, cloudiness_slots, weekdays, week_slots
condition_slots = [[None for i in range(num_segments)] for i in range(num_slots)]
temperature_slots = [[None for i in range(num_segments)] for i in range(num_slots)]
humidity_slots = [[None for i in range(num_segments)] for i in range(num_slots)]
wind_speed_slots = [[None for i in range(num_segments)] for i in range(num_slots)]
cloudiness_slots = [[None for i in range(num_segments)] for i in range(num_slots)]
weekdays = [None for i in range(num_slots)]
week_slots = [[None for i in range(8)] for i in range(7)]

def reset_forecast_data():
    global condition_slots, temperature_slots, humidity_slots, wind_speed_slots, cloudiness_slots, weekdays, week_slots
    condition_slots = [[None for i in range(num_segments)] for i in range(num_slots)]
    temperature_slots = [[None for i in range(num_segments)] for i in range(num_slots)]
    humidity_slots = [[None for i in range(num_segments)] for i in range(num_slots)]
    wind_speed_slots = [[None for i in range(num_segments)] for i in range(num_slots)]
    cloudiness_slots = [[None for i in range(num_segments)] for i in range(num_slots)]
    weekdays = [None for i in range(num_slots)]
    week_slots = [[None for i in range(8)] for i in range(7)]

def analyze_forecast(data):
    global condition_slots, temperature_slots, humidity_slots, wind_speed_slots, cloudiness_slots, weekdays
    day_index = -1
    segment_index = 0
    current_day = None
    count = wx.forecast_count(data)
    # data is pushed oldest first
    for x in xrange(count - 1, -1, -1):
        timestamp = wx.get_datetime(wx.forecast_timestamp(data, x))
        entry_day = timestamp.day
        if entry_day != current_day:
            day_index += 1
            segment_index = 0
            current_day = entry_day
            weekdays[day_index] = timestamp.weekday()
        condition_slots[day_index][segment_index] = wx.forecast_cond_id(data, x)
        temperature_slots[day_index][segment_index] = wx.forecast_temp(data, x)
        humidity_slots[day_index][segment_index] = wx.forecast_humidity(data, x)
        wind_speed_slots[day_index][segment_index] = wx.forecast_wind_speed(data, x)
        cloudiness_slots[day_index][segment_index] = wx.forecast_cloudiness(data, x)
        segment_index += 1
    fixup_data()

def fixup_data():
    global condition_slots, temperature_slots, humidity_slots, wind_speed_slots, cloudiness_slots

    # the first slot is the oldest data, its filler segments need to go first
    # so the data will need to be shifted

    # count number of filled and enpty slots
    count_of_filled = count_filled(condition_slots[0])
    need_to_fill = num_segments - count_of_filled

    # shift back the filled segments
    for y in range(count_of_filled):
        condition_slots[0][y + need_to_fill] = condition_slots[0][y]
        temperature_slots[0][y + need_to_fill] = temperature_slots[0][y]
        humidity_slots[0][y + need_to_fill] = humidity_slots[0][y]
        wind_speed_slots[0][y + need_to_fill] = wind_speed_slots[0][y]
        cloudiness_slots[0][y + need_to_fill] = cloudiness_slots[0][y]

    # add fillers for the non-filled segments
    for y in range(need_to_fill):
        condition_slots[0][y] = None
        temperature_slots[0][y] = None
        humidity_slots[0][y] = None
        wind_speed_slots[0][y] = None
        cloudiness_slots[0][y] = None

def map_to_weekdays(slots):
    # fill MTWTFSS week for familiar display
    for x in range(num_slots):
        weekday = weekdays[x]
        if weekday != None:
            week_slots[weekday] = slots[x]

    # shift to SMTWTFS
    carry_slot = week_slots[6]
    for x in range(6, 0, -1):
        week_slots[x] = week_slots[x-1]
    week_slots[0] = carry_slot

def send_week_slots(type_style):
    if double_size:
        day_spacer = "4:blk:"
        type_style = wc.weather_conditions[type_style] + ":rep:" 
    else:
        day_spacer = "2:blk:"
        type_style = wc.weather_conditions[type_style]   
    begin_display_sequence()
    lc.push_command(type_style + ":blk:")
    for x in xrange(7 -1, -1, -1):
        if x < 7-1:
            lc.push_command(day_spacer)
        for y in range(num_segments):
            style = week_slots[x][y]
            if ((x - 1) % 7) == wx.current_weekday():
                style += ":bre"
            cmd_text = style + ":"
            if double_size:
                cmd_text += cmd_text
            lc.push_command(cmd_text)
    lc.push_command("blk:" + type_style)
    lc.push_command()
    end_display_sequence()
    
def render_forecast_conditions():
    filler_data = "0,0," + str(minimum_lightness) + ":hsl"
    map_to_weekdays(condition_slots)
    for x in xrange(7 -1, -1, -1):
        for y in range(num_segments):
            condition = week_slots[x][y]
            if condition == None:
                week_slots[x][y] = filler_data
            else:
                week_slots[x][y] = wc.weather_conditions[int(condition)]

minimum_temperature = 40
maximum_temperature = 110
temperature_range = maximum_temperature - minimum_temperature

def hsl_color(hue, sat=255, lit=None):
    if lit == None:
        # default lightness is half brightness for mute colors
        lit = default_lightness / 2
    return str(hue) + "," + str(sat) + "," + str(lit) + ":hsl"

def render_forecast_temperature():
    filler_data = "0,0," + str(minimum_lightness) + ":hsl"
    map_to_weekdays(temperature_slots)
    for x in xrange(7 -1, -1, -1):
        for y in range(num_segments):
            temperature = week_slots[x][y]
            if temperature == None:
                week_slots[x][y] = filler_data
            else:
                temperature = int(temperature)
                #print "temp: " + str(temperature)
                
                # restrict to allowed temperature range
                temperature = max(minimum_temperature, temperature)
                temperature = min(maximum_temperature-1, temperature)

                # convert to 0-range_size
                temperature -= minimum_temperature
                # invert the so that a lower temperature is blue
                temperature = temperature_range - temperature
                #print "temp2: " + str(temperature)

                # convert to hue 0-255
                # hue = temperature * 3
                # hue = int(temperature * 255.0 / temperature_range)
                hue = temperature * 2
                #print "hue: " + str(hue)

                week_slots[x][y] = hsl_color(hue)

def render_forecast_humidity():
    filler_data = "0,0," + str(minimum_lightness) + ":hsl"
    sat = 255
    hue_min = 120
    hue_max = 240
    humidity_low_limit = 30
    map_to_weekdays(humidity_slots)
    for x in xrange(7 -1, -1, -1):
        for y in range(num_segments):
            humidity = week_slots[x][y]
            if humidity == None:
                week_slots[x][y] = filler_data
            else:
                humidity = int(humidity) 

                # trim range to 40%-100% to increase visibility of differences
                humidity = max(humidity_low_limit, humidity)
                humidity = (humidity - humidity_low_limit) / (100.0 - humidity_low_limit)

                # map humidity to hue-min thru hue-max
                hue_diff = hue_max - hue_min
                hue = hue_min + int(humidity * hue_diff)

                # map humidity to 0-max default lightness
                lightness_range = (default_lightness - minimum_lightness)
                lit = int(humidity * lightness_range) + minimum_lightness
                week_slots[x][y] = hsl_color(hue, sat, lit)

def render_forecast_wind_speed():
    filler_data = "0,0," + str(minimum_lightness) + ":hsl"
    map_to_weekdays(wind_speed_slots)
    for x in xrange(7 -1, -1, -1):
        for y in range(num_segments):
            wind_speed = week_slots[x][y]
            if wind_speed == None:
                week_slots[x][y] = filler_data
            else:
                wind_speed = int(wind_speed)
                if 0 <= wind_speed <= 1:
                    week_slots[x][y] = "grn:1:dim"
                elif 0 <= wind_speed <= 2:
                    week_slots[x][y] = "lgr:1:dim"
                elif 0 <= wind_speed <= 3:
                    week_slots[x][y] = "olv:1:dim"
                elif 0 <= wind_speed <= 5:
                    week_slots[x][y] = "yel"
                elif 0 <= wind_speed <= 7:
                    week_slots[x][y] = "amb"
                elif 0 <= wind_speed <= 10:
                    week_slots[x][y] = "org"
                elif 0 <= wind_speed <= 12:
                    week_slots[x][y] = "neo"
                elif 0 <= wind_speed <= 15:
                    week_slots[x][y] = "red"
                elif 0 <= wind_speed <= 17:
                    week_slots[x][y] = "red:1:brt"
                elif 0 <= wind_speed <= 20:
                    week_slots[x][y] = "red:1:brt:bla"
                elif 0 <= wind_speed <= 99:
                    week_slots[x][y] = "red:2:brt:bla"
                else:
                    week_slots[x][y] = filler_data

def render_forecast_cloudiness():
    filler_data = "0,0," + str(minimum_lightness) + ":hsl"
    hue = 220 # blue less purple
    map_to_weekdays(cloudiness_slots)
    for x in xrange(7 -1, -1, -1):
        for y in range(num_segments):
            cloudiness = week_slots[x][y]
            if cloudiness == None:
                week_slots[x][y] = filler_data
            else:
                cloudiness = int(cloudiness)
                if cloudiness == 0:
                    # if there are zero clouds, show an amber sun instead of blue sky
                    week_slots[x][y] = "yel"
                else:
                    # convert 0-100 to 100-0
                    cloudiness = 100 - cloudiness
                    # convert 100-0 to 255-0
                    sat = int(255 * (cloudiness / 100.0))
                    week_slots[x][y] = hsl_color(hue, sat)

def send_forecast_conditions():
    render_forecast_conditions()
    send_week_slots(2)

def send_forecast_temperature():
    render_forecast_temperature()
    send_week_slots(3)

def send_forecast_humidity():
    render_forecast_humidity()
    send_week_slots(4)

def send_forecast_wind_speed():
    render_forecast_wind_speed()
    send_week_slots(5)

def send_forecast_cloudiness():
    render_forecast_cloudiness()
    send_week_slots(6)

def reset_display():
    lc.command("pau:-1:sch:0:drw")

def setup_palette():
    lc.command("era:wht:pur:blu:grn:org:red:6,-1:cpy:era")

def continue_effects():
    lc.command("1:cnt")

def setup_display():
    reset_display()
    setup_palette()

## diagnostics

def write_json_str(filename, json_str):
    script_path = os.getcwd()
    filename = os.path.join(script_path, filename)
    ui.report_verbose_alt("Writing data to file: " + filename)
    with open(filename, "w") as text_file:
        text_file.write(json_str)

def prettify_json(json_data):
    return json.dumps(json_data, indent=4, sort_keys=True)

def write_daily_data(json_data):
    filename = "daily-" + str(wx.current_time()) + ".json"
    json_str = prettify_json(json_data)
    write_json_str(filename, json_str)

def write_forecast_data(json_data):
    filename = "forecast-" + str(wx.current_time()) + ".json"
    json_str = prettify_json(json_data)
    write_json_str(filename, json_str)


############################################################################

def setup():
    initialize()
    introduction()

num_displays = 5
global current_display_type, api_limiter
current_display_type = 0
api_limiter = 0

def get_latest_data():
    global daily_data, forecast_data
    daily_data = wx.get_daily_data()
    forecast_data = wx.get_forecast_data()

    if dump_mode:
        write_daily_data(daily_data)
        write_forecast_data(forecast_data)

API_LIMIT = 10

def time_to_get_data():
    global current_display_type, api_limiter           
    if current_display_type != 0:
        return False

    result = api_limiter == 0

    api_limiter += 1
    if api_limiter >= API_LIMIT:
        api_limiter = 0

    return result


def loop():
    global current_display_type

    if time_to_get_data():
        get_latest_data()

        if not suppress_spew:
            report_weather(daily_data)
            report_forecast(forecast_data)

    reset_forecast_data()
    analyze_forecast(forecast_data)    

    setup_display()

    if current_display_type == 0:
        send_forecast_conditions()
    if current_display_type == 1:
        send_forecast_temperature()
    if current_display_type == 2:
        send_forecast_humidity()
    if current_display_type == 3:
        send_forecast_wind_speed()
    if current_display_type == 4:
        send_forecast_cloudiness()

    continue_effects()

    current_display_type = (current_display_type + 1) % num_displays

############################################################################
############################################################################

if __name__ == '__main__':
    setup()
    while True:
        try:
            loop()
            ui.report_verbose("pausing until next update")
            time.sleep(update_freq)
        except requests.exceptions.ConnectionError:
            ui.report_error("connection error - retrying")
            time.sleep(retry_delay)
        except KeyboardInterrupt:
            sys.exit("\nExiting...\n")
        except Exception:
            raise


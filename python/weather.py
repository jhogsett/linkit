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

global app_description, verbose_mode, api_key, zip_code, update_freq, retry_delay, timezone_offset, min_api_delay, num_leds, suppress_spew, default_lightness
app_description = None
verbose_mode = None
api_key = None
zip_code = None
update_freq = None
retry_delay = None
timezone_offset = None
min_api_delay = None
num_leds = None
suppress_spew = None
default_lightness = None

def get_options():
    global verbose_mode, api_key, zip_code, update_freq, retry_delay, timezone_offset, min_api_delay, suppress_spew
    parser = argparse.ArgumentParser(description=app_description)
    parser.add_argument("-v", "--verbose", dest="verbose", action="store_true", help="display verbose info (False)")
    parser.add_argument("-k", "--apikey", dest="apikey", help="openweathermap.org api key")
    parser.add_argument("-z", "--zipcode", dest="zipcode", help="local postal zip code")
    parser.add_argument("-f", "--update_frequency", type=int, dest="update_freq", default=900, help="update freqency (seconds) (900)")
    parser.add_argument("-d", "--retry_delay", type=int, dest="retry_delay", default=5, help="connection retry delay (seconds) (5)")
    parser.add_argument("-t", "--timezone_offset", type=int, dest="timezone_offset", default=8, help="timezone offset from UTC (hours) (8)")
    parser.add_argument("-m", "--min_api_delay", type=int, dest="min_api_delay", default=1, help="minimum api call delay (seconds) (1)")
    parser.add_argument("-s", "--suppress_spew", dest="suppress_spew", action="store_true", help="supress period data display (False)")
    args = parser.parse_args()
    verbose_mode = args.verbose
    api_key = args.apikey
    zip_code = args.zipcode
    update_freq = args.update_freq
    retry_delay = args.retry_delay
    timezone_offset = args.timezone_offset
    min_api_delay = args.min_api_delay
    suppress_spew = args.suppress_spew

def initialize():
    global app_description, num_leds, default_lightness
    app_description = "LED Weather Forecaster v.0.0 12-21-2017"
    get_options()
    if not validate_options():
        sys.exit("\nExiting...\n")
    lc.begin(verbose_mode)
    lc.command("::pau")
    lc.command("::pau:era:flu")
    num_leds = lc.get_num_leds()
    default_lightness = lc.get_default_lightness()
    lc.command("cnt")

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
    report_verbose(intro_entry("Default Lightness", default_lightness))
    report_verbose()

    data = get_daily_data()
    report_info(intro_entry("City", daily_city(data)))
    report_info(intro_entry("Latitude", daily_lat(data)))
    report_info(intro_entry("Longitude", daily_lon(data)))
    report_info(intro_entry("Zip Code", zip_code))
    report_info(intro_entry("Update Frequency", update_freq))
    report_info(intro_entry("Number of LEDs", num_leds))

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

def tine_zone_timestamp(ts):
    return int(ts) - (timezone_offset * 60 * 60)

def get_datetime(ts):
    ts = tine_zone_timestamp(ts)
    return datetime.datetime.fromtimestamp(ts)

def format_unix_timestamp(ts):
    return format_datetime(get_datetime(ts))

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

def forecast_cloudiness(data, entry):
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
        print weather_entry(" Clouds", forecast_cloudiness(data, x)),
        print weather_entry(" Cond ID", forecast_cond_id(data, x)),
        print weather_entry(" Conditions", forecast_conditions(data, x)),
        print weather_entry(" Description", forecast_description(data, x))        

#def setup_palette():
#    lc.command("1:shf")

def begin_display_sequence():
    lc.command("::pau")
    lc.command("::pau:era")
    lc.flush_output()

def end_display_sequence():
    lc.command("cnt")
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
    count = forecast_count(data)
    # data is pushed oldest first
    for x in xrange(count - 1, -1, -1):
        timestamp = get_datetime(forecast_timestamp(data, x))
        entry_day = timestamp.day
        if entry_day != current_day:
            day_index += 1
            segment_index = 0
            current_day = entry_day
            weekdays[day_index] = timestamp.weekday()
        condition_slots[day_index][segment_index] = forecast_cond_id(data, x)
        temperature_slots[day_index][segment_index] = forecast_temp(data, x)
        humidity_slots[day_index][segment_index] = forecast_humidity(data, x)
        wind_speed_slots[day_index][segment_index] = forecast_wind_speed(data, x)
        cloudiness_slots[day_index][segment_index] = forecast_cloudiness(data, x)
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
    print weekdays
    for x in range(num_slots):
        weekday = weekdays[x]
        if weekday != None:
            week_slots[weekday] = slots[x]

    # shift to SMTWTFS
    carry_slot = week_slots[6]
    for x in range(6, 0, -1):
        week_slots[x] = week_slots[x-1]
    week_slots[0] = carry_slot

filler_data = wc.weather_conditions[1]

def send_week_slots(day_spacer):
    day_spacer = (wc.weather_conditions[day_spacer] + ":") * 4
    begin_display_sequence()
    for x in xrange(7 -1, -1, -1):
        lc.push_command(day_spacer)
        for y in range(num_segments):
            style = week_slots[x][y]
            lc.push_command((style + ":") * 2)
    lc.push_command(day_spacer)
    lc.push_command()
    end_display_sequence()
    
def render_forecast_conditions():
    map_to_weekdays(condition_slots)
    for x in xrange(7 -1, -1, -1):
        for y in range(num_segments):
            condition = week_slots[x][y]
            if condition == None:
                week_slots[x][y] = filler_data
            else:
                week_slots[x][y] = wc.weather_conditions[int(condition)]

minimum_temperature = 32
maximum_temperature = 102
temperature_range = maximum_temperature - minimum_temperature

def hsl_color(hue, sat=255, lit=None):
    if lit == None:
        # default lightness is half brightness for mute colors
        lit = default_lightness / 2
    return str(hue) + "," + str(sat) + "," + str(lit) + ":hsl"

def render_forecast_temperature():
    map_to_weekdays(temperature_slots)
    for x in xrange(7 -1, -1, -1):
        for y in range(num_segments):
            temperature = week_slots[x][y]
            if temperature == None:
                week_slots[x][y] = filler_data
            else:
                temperature = int(temperature)
                # restrict to 32-111
                temperature = max(minimum_temperature, temperature)
                temperature = min(maximum_temperature-1, temperature)
                # convert to 0-79
                temperature -= minimum_temperature
                # convert to 79-0 so that a lower temperature is blue
                temperature = temperature_range - temperature
                # convert to hue 237-0
                hue = temperature * 3
                week_slots[x][y] = hsl_color(hue)

#def render_forecast_humidity():
#    map_to_weekdays(humidity_slots)
#    for x in xrange(7 -1, -1, -1):
#        for y in range(num_segments):
#            humidity = week_slots[x][y]
#            if humidity == None:
#                week_slots[x][y] = filler_data
#            else:
#                humidity = int(humidity)
#                # map 0-100 humidity to 0-300 hue (last 60 degrees head back to red)
#                hue = humidity * 3
#                week_slots[x][y] = hsl_color(hue)

def render_forecast_humidity():
    hue = 220 # blue less purple
    sat = 255
    map_to_weekdays(humidity_slots)
    for x in xrange(7 -1, -1, -1):
        for y in range(num_segments):
            humidity = week_slots[x][y]
            if humidity == None:
                week_slots[x][y] = filler_data
            else:
                humidity = int(humidity)
                # map 0-100 humidity to 0-max default lightness
                lit = (humidity / 100.0) * default_lightness
                week_slots[x][y] = hsl_color(hue, sat, lit)

maximum_wind_speed = 15

def render_forecast_wind_speed():
    map_to_weekdays(wind_speed_slots)
    for x in xrange(7 -1, -1, -1):
        for y in range(num_segments):
            wind_speed = week_slots[x][y]
            if wind_speed == None:
                week_slots[x][y] = filler_data
            else:
                wind_speed = int(wind_speed)
                if wind_speed < 1:
                    # show nothing if there's no wind
                    week_slots[x][y] = filler_data
                else:
                    # assume 0-30 MPH
                    wind_speed = min(maximum_wind_speed, wind_speed)
                    # map to 0-300
                    hue = wind_speed * 20
                    week_slots[x][y] = hsl_color(hue)

#def render_forecast_cloudiness():
#    map_to_weekdays(cloudiness_slots)
#    for x in xrange(7 -1, -1, -1):
#        for y in range(num_segments):
#            cloudiness = week_slots[x][y]
#            if cloudiness == None:
#                week_slots[x][y] = filler_data
#            else:
#                cloudiness = int(cloudiness)
#                # if there are no clouds, show nothing
#                if cloudiness < 1:
#                    week_slots[x][y] = filler_data
#                else:
#                    # map 0-100 cloudiness to 0-300 hue (last 60 degrees head back to red)
#                    hue = cloudiness * 3
#                    week_slots[x][y] = hsl_color(hue)

#def render_forecast_cloudiness():
#    hue = 0 # red
#    sat = 0 # b&w only
#    map_to_weekdays(cloudiness_slots)
#    for x in xrange(7 -1, -1, -1):
#        for y in range(num_segments):
#            cloudiness = week_slots[x][y]
#            if cloudiness == None:
#                week_slots[x][y] = filler_data
#            else:
#                cloudiness = int(cloudiness)
#                lit = (cloudiness / 100.0) * (default_lightness / 2)
#                week_slots[x][y] = hsl_color(hue, sat, lit)

def render_forecast_cloudiness():
    hue = 220 # blue less purple
    map_to_weekdays(cloudiness_slots)
    for x in xrange(7 -1, -1, -1):
        for y in range(num_segments):
            cloudiness = week_slots[x][y]
            if cloudiness == None:
                week_slots[x][y] = filler_data
            else:
                cloudiness = int(cloudiness)
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



############################################################################

def setup():
    initialize()
    introduction()

num_displays = 5
global current_display_type
current_display_type = 0

def loop():
    global current_display_type

    daily_data = get_daily_data()
    forecast_data = get_forecast_data()

    if not suppress_spew:
        report_weather(daily_data)
        report_forecast(forecast_data)

    reset_forecast_data()
    analyze_forecast(forecast_data)    

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

    current_display_type = (current_display_type + 1) % num_displays


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


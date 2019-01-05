#!/usr/bin/python

import sys
import time
import argparse
import terminal_colors as tc
import app_ui as ui
import led_command as lc
from kelvin_to_rgb import convert_K_to_RGB
from subprocess import call

global app_description, verbose_mode, kelvin_start, kelvin_end, num_steps, brightness_start, brightness_end, step_time, kelvin_step, brightness_step, kelvin_current, brightness_current, dry_run, factor, send_to_led, send_to_udp, step_number, final_command
app_description = None
verbose_mode = None
kelvin_start = None
kelvin_end = None
num_steps = None
brightness_start = None
brightness_end = None
step_time = None
factor = None
dry_run = None
send_to_led = None
send_to_udp = None
step_number = None
final_command = None

kelvin_step = None
brightness_step = None
kelvin_current = None
brightness_current = None

# add factor, dry run

def get_options():
    global app_description, verbose_mode, kelvin_start, kelvin_end, num_steps, brightness_start, brightness_end, step_time, dry_run, factor, send_to_led, send_to_udp, final_command
    app_description = "Awakening Sunrise - Apollo Lighting System v.0.0 12-0-18"

    parser = argparse.ArgumentParser(description=app_description)
    parser.add_argument("-v", "--verbose", dest="verbose", action="store_true", help="display verbose info (False)")
    parser.add_argument("-n", "--num-steps", type=int, dest="num_steps", default=30, help="number of steps (30)")
    parser.add_argument("-t", "--step_time", type=int, dest="step_time", default=60, help="seconds for each step (60)")
    parser.add_argument("-ks", "--kelvin-start", type=int, dest="kelvin_start", default=1000, help="starting color temperature K (1000)")
    parser.add_argument("-ke", "--kelvin-end", type=int, dest="kelvin_end", default=5000, help="ending color temperature K (5000)")
    parser.add_argument("-bs", "--brightness-start", type=int, dest="brightness_start", default=1, help="starting brightness %% (1)")
    parser.add_argument("-be", "--brightness-end", type=int, dest="brightness_end", default=50, help="ending brightness %% (50)")
    parser.add_argument("-r", "--dry-run", dest="dry_run", action="store_true", help="process the awakening values but don't send the results (False)")
    parser.add_argument("-f", "--rgb-factor", type=int, dest="factor", default=10, help="rgb value divisor (10)")
    parser.add_argument("-l", "--send-to-led", dest="send_to_led", action="store_true", help="send the awakening values to the LED device (False)")
    parser.add_argument("-b", "--broadcast", dest="send_to_udp", action="store_true", help="send the awakening values to the network via UDP (False)")
    parser.add_argument("-c", "--final-command", dest="final_command", default="0:lev:10:run", help="LED command string to send when done (0:lev:10:run)")

    args = parser.parse_args()
    verbose_mode = args.verbose
    num_steps = args.num_steps
    step_time = args.step_time
    kelvin_start = args.kelvin_start
    kelvin_end = args.kelvin_end
    brightness_start = args.brightness_start
    brightness_end = args.brightness_end
    dry_run = args.dry_run
    factor = args.factor
    send_to_led = args.send_to_led
    send_to_udp = args.send_to_udp
    final_command = args.final_command

def validate_options():
    pass

def introduction():
    ui.app_description(app_description)

    ui.report_verbose("verbose mode")
    ui.report_verbose("number of steps: " + str(num_steps))
    ui.report_verbose("step time: " + str(step_time) + " seconds")
    ui.report_verbose("starting color temperature: " + str(kelvin_start) + "K")
    ui.report_verbose("ending color temperature: " + str(kelvin_end) + "K")
    ui.report_verbose("starting brightness: " + str(brightness_start) + "%")
    ui.report_verbose("ending brightness: " + str(brightness_end) + "%")
    ui.report_verbose("rgb factor: " + str(factor))
    ui.report_verbose("send to LED device: " + str(send_to_led))
    ui.report_verbose("broadcast to LED devices: " + str(send_to_udp))
    ui.report_verbose("dry run: " + str(dry_run))
    ui.report_verbose()

def initialize():
    global kelvin_step, brightness_step, kelvin_current, brightness_current, step_number
    get_options()
    validate_options()
    ui.begin(verbose_mode)
    introduction()
    lc.begin(verbose_mode)

    kelvin_step = (kelvin_end-kelvin_start) / (num_steps * 1.0)
    brightness_step = (brightness_end-brightness_start) / (num_steps * 1.0)

    ui.report_verbose("color temperature step: " + str(kelvin_step))
    ui.report_verbose("brightness step: " + str(brightness_step))

    kelvin_current = kelvin_start
    brightness_current = brightness_start
    step_number = 0

    on_start()

def on_start():
    send_to_led_device("app")
    broadcast("app")

def on_complete():
    send_to_led_device(final_command)
    broadcast(final_command)

def send_to_led_device(command_str):
    ui.report_verbose("sending to LED device: " + command_str)
    if send_to_led:
        if not dry_run:
            lc.command(command_str)

def broadcast(command_str):
    command_line = "/root/dev/linkit/python/udp_send.py " + command_str + " &"
    ui.report_verbose("broadcasting: " + command_line)
    if send_to_udp:
        if not dry_run:
            call(command_line, shell=True)

############################################################################

def loop():
    do_broadcast()
    next_step()
    #exit_if_done()
    wait_step()
    exit_if_done()

def do_broadcast():
    r, g, b = convert_K_to_RGB(kelvin_current)
    r = r / factor
    g = g / factor
    b = b / factor

#    command_str = str(int(brightness_current)) + ":lev:" + str(int(r)) + "," + str(int(g)) + "," + str(int(b)) + ":rgb:flo:flu"
    command_str = "0:-5," + str(int(brightness_current)) + ":lev:" + str(int(r)) + "," + str(int(g)) + "," + str(int(b)) + ":rgb:flo:flu"

    if step_number == 0:
        ui.report_info("Initial Step")
    else:
        ui.report_info("Step #" + str(step_number))

    ui.report_info("Current color temperature: " + str(int(kelvin_current)) + "K")
    ui.report_info("Current brightness: " + str(int(brightness_current)) + "%%")

    send_to_led_device(command_str)
    broadcast(command_str)

    print

def next_step():
    global kelvin_current, brightness_current, step_number
    kelvin_current += kelvin_step
    brightness_current += brightness_step
    step_number += 1

def exit_if_done():
    if kelvin_current > kelvin_end:
        ui.report_info("Awakening complete!")
        on_complete()
        sys.exit()

def wait_step():
    time.sleep(step_time)

############################################################################
############################################################################

if __name__ == '__main__':
    initialize()
    while True:
        try:
            loop()
        except KeyboardInterrupt:
            sys.exit("\nExiting...\n")
        except Exception:
            raise


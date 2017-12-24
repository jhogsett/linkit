import sys
import serial
import time

response_wait = 0.1
global s, verbose_mode, cmd
s = None
verbose_mode = False
cmd = ""
max_command_max = 30

def begin(verbose=False):
    global s,verbose_mode
    s = serial.Serial("/dev/ttyS0", 115200)
    verbose_mode = verbose
    flush_output()
    flush_input()

def flush_input():
    s.flushInput()

def flush_output():
    s.flushOutput()

def wait_for_ack():
    while s.inWaiting() <= 0:
        pass
    time.sleep(response_wait);
    while s.inWaiting() > 0:
        s.read(s.inWaiting())
#        print s.read(s.inWaiting()),
#    print

def wait_for_int():
    while s.inWaiting() <= 0:
        pass
    time.sleep(response_wait);
    intstr = ""
    while s.inWaiting() > 0:
        intstr = intstr + s.read(s.inWaiting())
    try:
        return int(intstr[:-1])
    except ValueError:
        print "whoops " + intstr
        return 0

def wait_for_str():
    while s.inWaiting() <= 0:
        pass
    time.sleep(response_wait);
    str = ""
    while s.inWaiting() > 0:
        str = str + s.read(s.inWaiting())
    return str[:-1]

def send_command(cmd_text):
    if verbose_mode:
        print "sending: " + cmd_text
    s.write((cmd_text + ':').encode())

def command(cmd_text):
    send_command(cmd_text)
    wait_for_ack()

def command_int(cmd_text):
    send_command(cmd_text)
    return wait_for_int()

def command_str(cmd_text):
    send_command(cmd_text)
    return wait_for_str()

def write(text):
    sys.stdout.write(text)
    sys.stdout.flush()

def get_device_config(config):
    return command_str("0," + str(config) + ":tst")

def get_num_leds():
    return get_device_config(0)

#def get_palette_size():
#    return get_device_config(1)
#
#def get_offset():
#    return get_device_config(2)
#
#def get_window():
#    return get_device_config(3)
#
#def get_default_brightness():
#    return get_device_config(4)
#
#def get_minimum_brightness():
#    return get_device_config(5)
#
#def get_reverse():
#    return get_device_config(6)
#
#def get_default_fade_rate():
#    return get_device_config(7)
#
#def get_fade_rate():
#    return get_device_config(8)
#
#def get_mapping_enabled():
#    return get_device_config(9)

def push_command(cmd_text=None):
    global cmd
    # without argument, send accumulated command if any
    if cmd_text == None:
        if len(cmd) > 0:
            command(cmd)
            cmd = ""
        return
    cmd = cmd + cmd_text
    if len(cmd) > max_command_max:
        command(cmd)
        cmd = ""


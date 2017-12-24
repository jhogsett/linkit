import sys
import serial
import time

response_wait = 0.1
global s
s = None

def begin():
    global s
    s = serial.Serial("/dev/ttyS0", 115200)

def flush_input():
    s.flushInput()

def wait_for_ack():
    while s.inWaiting() <= 0:
        pass
    time.sleep(response_wait);
    while s.inWaiting() > 0:
        print s.read(s.inWaiting()),
    print

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

def command(cmd_text):
    s.write((cmd_text + ':').encode())
    wait_for_ack()

def command_int(cmd_text):
    s.write((cmd_text + ':').encode())
    return wait_for_int()

def command_str(cmd_text):
    s.write((cmd_text + ':').encode())
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

	













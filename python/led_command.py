import sys
import serial
import time

response_wait = 0.01
global s, verbose_mode, cmd, max_command_chars
s = None
verbose_mode = False
cmd = ""
max_command_chars = None

def begin(verbose=False):
    global s, verbose_mode, max_command_chars
    s = serial.Serial("/dev/ttyS0", 115200)
    verbose_mode = verbose
    flush_output()
    flush_input()
    max_command_chars = get_max_string_length()
    if max_command_chars == None or max_command_chars == 0:
        raise StandardError("unable to get maximum command chars")
    max_command_chars -= 1

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
    return command_int("0," + str(config) + ":tst")

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

def get_default_lightness():
    return get_device_config(10)

def get_minimum_lightness():
    return get_device_config(11)

def get_max_string_length():
    return get_device_config(12)

def push_command(cmd_text=None):
    global cmd

    # without argument, send accumulated command if any
    if cmd_text == None:
        if len(cmd) > 0:
            command(cmd)
            cmd = ""
        return

    if len(cmd_text) > max_command_chars:
        raise StandardError("command length exceeds maximum. command: " + cmd_text + "length: " + str(len(cmd_text)) + " max length: " + str(max_command_chars))

    if len(cmd) + len(cmd_text) > max_command_chars:
        # adding new comand text would exceed maximum
        # send the accumulated commands and clear the buffer
        command(cmd)
        cmd = cmd_text
    else:
        # add this command text to the buffer
        cmd = cmd + cmd_text

def attention(erase=True):
    command("::pau")
    if erase:
        command("::pau:era:flu")
    else:
        command("::pau")

def set_macro(macro, macro_text, expected_bytes, debug_mode):
    if debug_mode:
      print "macro " + str(macro) + ": ",

    bytes = command_int(str(macro) + ":set:" + macro_text)

    if debug_mode:
        print str(bytes) + " bytes"
        print command_str("1," + str(macro) + ":tst")

    else:
        if expected_bytes > 0 and expected_bytes != bytes:
            raise StandardError("Wrong number of bytes received for macro %s" % str(macro)) 

    return bytes

def run_macro(macro):
    command("cnt:%s:run" % macro)

def stop_all():
    attention()
    command("stp")


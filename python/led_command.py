import sys
import serial
import time
import math
import struct

slow_response_wait = 0.15
fast_response_wait = 0.01

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
        #raise StandardError("unable to get maximum command chars")
        # just use a default
        max_command_chars = 60
    max_command_chars -= 1

def flush_input():
    s.flushInput()

def flush_output():
    s.flushOutput()

def wait_for_ack(slow=False):
    while s.inWaiting() <= 0:
        pass
    if slow:
        response_wait = slow_response_wait
    else:
        response_wait = fast_response_wait
    time.sleep(response_wait);
    while s.inWaiting() > 0:
        s.read(s.inWaiting())
#        print s.read(s.inWaiting()),
#    print

def wait_for_int(slow=False):
    while s.inWaiting() <= 0:
        pass
    if slow:
        response_wait = slow_response_wait
    else:
        response_wait = fast_response_wait
    time.sleep(response_wait);
    intstr = ""
    while s.inWaiting() > 0:
        intstr = intstr + s.read(s.inWaiting())
    try:
        return int(intstr[:-1])
    except ValueError:
        print "whoops " + intstr
        return 0

def wait_for_str(slow=False):
    while s.inWaiting() <= 0:
        pass
    if slow:
        response_wait = slow_response_wait
    else:
        response_wait = fast_response_wait
    time.sleep(response_wait);
    str = ""
    while s.inWaiting() > 0:
        str = str + s.read(s.inWaiting())
    return str[:-1]

def send_command(cmd_text):
    if verbose_mode:
        print "sending: " + cmd_text
    s.write((cmd_text + ':\0:').encode())

def command(cmd_text, slow=False):
    send_command(cmd_text)
    wait_for_ack(slow)

def command_int(cmd_text, slow=False):
    send_command(cmd_text)
    return wait_for_int(slow)

def command_str(cmd_text, slow=False):
    send_command(cmd_text)
    return wait_for_str(slow)

def write(text):
    sys.stdout.write(text)
    sys.stdout.flush()

def get_device_config(config):
    return command_int("0," + str(config) + ":tst")

def set_device_config(config, value):
    command(str(config) + "," + str(value) + ":cfg")

def is_enabled(config):
    return get_device_config(config) == 1

def get_num_leds():
    return get_device_config(0)

def get_palette_size():
    return get_device_config(1)

def get_offset():
    return get_device_config(2)

def get_window():
    return get_device_config(3)

def get_default_brightness():
    return get_device_config(4)

def get_minimum_brightness():
    return get_device_config(5)

def get_reverse():
    return get_device_config(6)

def get_default_fade_rate():
    return get_device_config(7)

def get_fade_rate():
    return get_device_config(8)

def get_mapping_enabled():
    return get_device_config(9)

def get_default_lightness():
    return get_device_config(10)

def get_minimum_lightness():
    return get_device_config(11)

def get_max_string_length():
    return get_device_config(12)

def get_test_framework_enabled():
    return get_device_config(13)

def get_extra_shuffles_enabled():
    return get_device_config(14)

def get_blend_enabled():
    return get_device_config(15)

def get_num_macro_bytes():
    return get_device_config(16)

def get_num_memory_macros():
    return get_device_config(17)

def get_num_eeprom_macros():
    return get_device_config(18)

def get_first_eeprom_macro():
    return get_device_config(19)

def get_last_eeprom_macro():
    return get_first_eeprom_macro() + int(math.ceil(1024.0 / get_num_macro_bytes()) - 1);

def get_num_sequencers():
    return get_device_config(20)

def get_num_fine_zones():
    return get_device_config(21)

def get_last_macro_bytes():
    bytes_per_macro = get_num_macro_bytes()
    return (1024 - ((1024 / bytes_per_macro) * bytes_per_macro))

# device parameteruseful for compiling and device programming
def get_device_profile():
    return {
        "CHAR-BUFFER-SIZE": get_max_string_length(),
        "NUM-LEDS": get_num_leds(),
        "NUM-MACROS": get_num_eeprom_macros(),
        "TOTAL-MACRO-BYTES": 1024,
        "NUM-MACRO-BYTES": get_num_macro_bytes(),
        "LAST-MACRO-BYTES": get_last_macro_bytes(),
        "NUM-SEQUENCERS": get_num_sequencers(),
        "NUM-FINE-ZONES": get_num_fine_zones(),
        "NUM-PALETTE-COLORS": get_palette_size(),
        "START-MACRO": get_first_eeprom_macro(),
        "END-MACRO": get_last_eeprom_macro(),
        "CHAR-BUFFER-SIZE": get_max_string_length()
    }        

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

def pause():
  command("3:pau")

def resume():
  command("3:cnt")

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
        #print str(bytes) + " bytes"
        macro_bytes = command_str("1," + str(macro) + ":tst")
        #print "Macro bytes: " + str(macro_bytes)
        print "Macro: " + lc.get_macro(macro)
    else:
        if expected_bytes > 0 and expected_bytes != bytes:
            raise StandardError("Wrong number of bytes received for macro %s" % str(macro)) 
    return bytes

def run_macro(macro):
    command("1:pau:2:cnt:%s:run" % macro)

def get_macro_raw(macro):
    return command_str("1," + str(macro) + ":tst")

def get_macro(macro):
    macro_bytes = get_full_macro_bytes(macro)
    return str(macro) + ":set:" + translate_macro_bytes(macro_bytes)

def get_macros():
    result = []
    last_macro = get_last_eeprom_macro()
    for i in range(0, last_macro + 1):
      result.append(get_macro(i))
    return result

def get_macro_bytes(macro):
    bytes_string = command_str("1," + str(macro) + ":tst")[:-1]
    byte_strings = bytes_string.split(",")
    result = []
    for byte_string in byte_strings:
      #print "byte string: " + byte_string
      result.append(int(byte_string))
    return result

def get_full_macro_bytes(macro):
  result = []
  buffer = []
  macro_size = get_num_macro_bytes()
  last_macro = get_last_eeprom_macro()
  current_macro = macro
  index = 0
  script_terminator_found = False
  macro_bytes_max = 0
  while True:
    if current_macro > last_macro:
      raise ValueError("Ran out of macro numbers getting a consolidated macro")
    macro_bytes = get_macro_bytes(current_macro)
    buffer += macro_bytes
    macro_bytes_max += macro_size
    #print str(macro_bytes)
    #index = 0
    #script_terminator_found = False
    while(index < macro_bytes_max):
      #print "index: " + str(index)
      #print "len: " + str(len(macro_bytes))
      #print "max: " + str(macro_bytes_max)
      byte = buffer[index]
      if byte == 255 or byte == 0:
        script_terminator_found = True
        break
      if is_arg_marker(byte):
        format, size = get_arg_decode_info(byte)
        # index could be the last byte in the buffer
        # at least one more byte is needed
        if index + size >= macro_bytes_max:
          # break the inner loop to get the next set of bytes
          # and don't increment the index so the byte is checked again
          break
        result.append(byte)
        index += 1
        for i in range(0, size):
          #print "INDEX: " + str(index)
          byte = buffer[index]
          result.append(byte)
          index += 1
      else:
        result.append(byte)
        index += 1
    if script_terminator_found:
      break;
    current_macro += 1
  return result

def get_buffer(start, count, slow=False):
  return command_str("2," + str(start) + "," + str(count) + ":tst", slow)

def get_render(start, count, slow=False):
  return command_str("3," + str(start) + "," + str(count) + ":tst", slow)

def get_effect(start, count, slow=False):
  return command_str("4," + str(start) + "," + str(count) + ":tst", slow)

def get_palette(start, count, slow=False):
  return command_str("5," + str(start) + "," + str(count) + ":tst", slow)

def do_test_process(process, data):
  command_str("6," + str(process) + "," + str(data) + ":tst")

def set_random_seed(seed):
  do_test_process(3, seed)

def get_accumulator():
  return command_str("7:tst")

def randomize_palette():
  command("shf")    

def reset_palette():
  command("1:shf")

def stop_all():
    attention()
    command("stp")

#define MACRO_END_MARKER 0xff
#define MACRO_ARG1_MARKER 0xf9
#define MACRO_ARG2_MARKER 0xfa
#define MACRO_ARG3_MARKER 0xfb
#define MACRO_ARG4_MARKER 0xfc
#define MACRO_ARG5_MARKER 0xfd
#define MACRO_ARG6_MARKER 0xfe

def is_arg_marker(byte):
  return byte >= 240 and byte <= 254

def is_macro_end_marker(byte):
  return byte == 255

def get_arg_decode_info(marker):
  decode_args = {
    249: { "format": "B", "size": 1 },
    250: { "format": "h", "size": 2 },
    251: { "format": "hB", "size": 3 },
    252: { "format": "hh", "size": 4 },
    253: { "format": "hhB", "size": 5 },
    254: { "format": "hhh", "size": 6}
  }
  format = decode_args[marker]["format"]
  size = decode_args[marker]["size"]
  return format, size

def decode_args(bytes):
  leftover_bytes = []
  #print "bytes: " + str(bytes) #@@@

  marker = bytes[0]
  format, size = get_arg_decode_info(marker)
  remaining_bytes = bytes[1:]
  #print "remaining bytes: " + str(remaining_bytes)
  value_bytes = remaining_bytes[:size]
  #print "value bytes: " + str(value_bytes)
  packed_as_bytes = struct.pack(str(size) + "B", *tuple(value_bytes))
  values = struct.unpack(format, packed_as_bytes)
  #print "values: " + str(values)
  args_string = ""
  for value in values:
    args_string += str(value) + ","
  return args_string[:-1], size

def translate_macro_bytes(macro_bytes):
    commands_cutoff = 240
    macro = []
    index = 0
    while index < len(macro_bytes):
        byte = macro_bytes[index]
        if byte == 255:
          break;
        if is_arg_marker(byte):
          args_string, size = decode_args(macro_bytes[index:])
          macro.append(args_string)
          index += size
        else:
          macro.append(lookup_command(byte))
        index += 1
    return ":".join(macro)

def lookup_command(byte):
  try:
    command = commands()[byte]
  except KeyError:
    command = "???"
  return command

def commands():
    return {
        1: "flu",
        2: "rst",
        3: "era",
        4: "run",
        5: "pal",
        6: "pos",
        7: "rot",
        8: "rep",
        9: "cpy",
        10: "flo",
        11: "mir",
        12: "dis",
        13: "zon",
        14: "win",
        15: "off",
        16: "rev",
        17: "rgb",
        18: "hsl",
        19: "red",
        20: "org",
        21: "yel",
        22: "grn",
        23: "blu",
        24: "pur",
        25: "cyn",
        26: "mag",
        27: "lbl",
        28: "lgr",
        29: "sea",
        30: "pnk",
        31: "amb",
        32: "olv",
        33: "sky",
        34: "tur",
        35: "lav",
        36: "ros",
        37: "dgr",
        38: "gry",
        39: "wht",
        40: "tun",
        41: "neo",
        42: "sod",
        43: "blk",
        44: "rnd",
        45: "dyn",
        46: "ble",
        47: "dim",
        48: "brt",
        49: "bli",
        50: "bl1",
        51: "bl2",
        52: "bl3",
        53: "bl4",
        54: "bl5",
        55: "bl6",
        56: "bla",
        57: "blb",
        58: "bld",
        59: "bre",
        60: "brd",
        61: "sfd",
        62: "ffd",
        63: "sto",
        64: "rcl",
        65: "psh",
        66: "pau",
        67: "cnt",
        68: "clr",
        69: "lev",
        70: "art",
        71: "cfa",
        72: "tst",
        73: "cfg",
        74: "pin",
        75: "sch",
        76: "car",
        77: "set",
        78: "del",
        79: "stp",
        80: "rng",
        81: "rps",
        82: "xyp",
        83: "shf",
        84: "sbl",
        85: "seq",
        86: "sqs",
        87: "swc",
        88: "sws",
        89: "snw",
        90: "csh",
        91: "css",
        92: "csl",
        93: "fan",
        94: "app",
        95: "add",
        96: "sub",
        97: "mul",
        98: "div",
        99: "mod",
        100: "dif",
        101: "avg",
        102: "drw",
        103: "pop"
    }


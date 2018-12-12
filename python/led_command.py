import sys
import serial
import time
import math
import struct

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
        #raise StandardError("unable to get maximum command chars")
        # just use a default
        max_command_chars = 60
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
    s.write((cmd_text + ':\0:').encode())

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
        4: "rot",
        5: "rep",
        6: "cpy",
        7: "flo",
        8: "mir",
        9: "dis",
        10: "zon",
        11: "win",
        12: "off",
        13: "rev",
        14: "rgb",
        15: "hsl",
        16: "red",
        17: "org",
        18: "yel",
        19: "grn",
        20: "blu",
        21: "pur",
        22: "cyn",
        23: "mag",
        24: "lbl",
        25: "lgr",
        26: "sea",
        27: "pnk",
        28: "amb",
        29: "olv",
        30: "sky",
        31: "tur",
        32: "lav",
        33: "ros",
        34: "dgr",
        35: "gry",
        36: "wht",
        37: "tun",
        38: "neo",
        39: "sod",
        40: "blk",
        41: "rnd",
        42: "dyn",
        43: "ble",
        44: "max",
        45: "dim",
        46: "brt",
        47: "sta",
        48: "bli",
        49: "bl1",
        50: "bl2",
        51: "bl3",
        52: "bl4",
        53: "bl5",
        54: "bl6",
        55: "bla",
        56: "blb",
        57: "bld",
        58: "bre",
        59: "brd",
        60: "sfd",
        61: "ffd",
        62: "sto",
        63: "rcl",
        64: "psh",
        65: "pau",
        66: "cnt",
        67: "clr",
        68: "lev",
        69: "fad",
        70: "art",
        71: "cfa",
        72: "blr",
        73: "efr",
        74: "tst",
        75: "cfg",
        76: "pin",
        77: "sch",
        78: "car",
        79: "set",
        80: "run",
        81: "del",
        82: "stp",
        83: "rng",
        84: "pos",
        85: "rps",
        86: "xyp",
        87: "pal",
        88: "shf",
        89: "sbl",
        90: "seq",
        91: "sqs",
        92: "swc",
        93: "sws",
        94: "snw",
        95: "csh",
        96: "css",
        97: "csl",
        98: "fan",
        99: "app",
        100: "add",
        101: "sub",
        102: "mul",
        103: "div",
        104: "mod",
        105: "dif",
        106: "avg",
        107: "drw",
        108: "pop"
    }


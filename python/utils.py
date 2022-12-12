
import os
from subprocess import call
import sys
import time
import random
import string
import hashlib
import socket
import fcntl

global script_directory
script_directory = None

def begin(use_tty = True, script_file = None):
    global script_directory, select, tty, termios
    script_directory = get_path(script_file)
    if use_tty:
        import select
        import tty
        import termios

def get_script_directory():
    return script_directory

def load_file(filename, default_ext=".???"):
    file_lines = []

    # add presumed extension
    if not filename.endswith(default_ext):
        filename = filename + default_ext

    # save a path to the file for use later
    # file_path = os.path.dirname(filename)
    # this would be helpful if processing includes

    with open(filename) as file:
        file_lines = [line.strip() for line in file]

    return file_lines

def get_pair(line, split_on="-"):
  line = line.strip()
  args = line.split(split_on)
  if len(args) == 0:
    return [None, None]
  elif len(args) == 1:
    return [args[0].strip(), None]
  elif len(args) == 2:
    return [args[0].strip(), args[1].strip()]

def strip_whitespace(lines):
  new_lines = []
  for line in lines:
    line = line.strip()
    if len(line) > 0:
      new_lines.append(line)
  return new_lines

def strip_comments(lines, comment_marker="#"):
  new_lines = []
  for line in lines:
    if comment_marker in line:
      parts = line.split(comment_marker)
      keep = parts[0].strip()
      if len(keep) > 0:
          new_lines.append(keep)
    else:
      new_lines.append(line)
  return new_lines

def run_command(command_line):
  call(command_line, shell=True)

def input_waiting():
  return select.select([sys.stdin], [], [], 0) == ([sys.stdin], [], [])

def get_input(wait_seconds):
  char = None
  start_time = time.time()
  target_time = start_time + wait_seconds
  old_settings = termios.tcgetattr(sys.stdin)
  try:
    tty.setcbreak(sys.stdin.fileno())
    while time.time() < target_time and char == None:
      if input_waiting():
        char = sys.stdin.read(1)
        #if c == '\x1b': # x1b is ESC
        #  break
  finally:
    termios.tcsetattr(sys.stdin, termios.TCSADRAIN, old_settings)
  return char

def merge_dicts(x, y):
    z = x.copy() 
    z.update(y)
    return z

def randomize(seed=0, max=4294967296):
    if seed == 0:
        seed = random.randint(1, max)
    random.seed(seed)
    return seed

# todo: also look in the current directory if nothing else works
def locate_file(filename, default_extension, default_directory = None):
  if default_directory == None:
    default_directory = script_directory
  orig_filename = filename
  if not filename.endswith(default_extension):
    filename = filename + default_extension
  if not os.path.isfile(filename):
    filename = os.path.basename(filename)
    filename = os.path.join(default_directory, filename)
    if not os.path.isfile(filename):
      raise ValueError("(utils.py) File '" + orig_filename + "' cannot be found.")
  return filename

def get_filename(file_path):
  return os.path.basename(file_path)

def get_extension(file_path):
  filename, extension = os.path.splitext(file_path)
  return extension

def get_filename_only(file_path):
  filename, extension = os.path.splitext(file_path)
  return filename

def get_path(file_path):
  return os.path.dirname(os.path.abspath(file_path))

# locate the start and end positions of a delimited portion of a string
# returns start, end
def locate_delimiters(line, delimiters={}, outer=False):
    start_delimiter = delimiters.keys()[0]
    end_delimiter = delimiters[start_delimiter]

    start = -1
    end = -1
    if start_delimiter in line:
        start = line.find(start_delimiter)
        if end_delimiter in line[start + len(start_delimiter):]:
            if outer == False:
                end = line.find(end_delimiter, start + 1)
            else:
                end = line.rfind(end_delimiter, start + 1)
    return start, end

def _cut_contents(line, start_delimiter, end_delimiter, start, end):
    return line[start + len(start_delimiter):end].strip()

# pass in line and two delimiters, get back contents within
# delimiters specified as one or two characters
def extract_contents(line, delimiters={}, outer=False):
    start_delimiter = delimiters.keys()[0]
    end_delimiter = delimiters[start_delimiter]

    line = line.strip()
    if len(line) == 0:
        return ''
    start, end = locate_delimiters(line, {start_delimiter : end_delimiter}, outer)
    if start != -1 and end != -1:
        return _cut_contents(line, start_delimiter, end_delimiter, start, end)
    return ''

# pass in line and two delimiters, get back list of arguments within
# delimiters specified as one or two characters
def extract_args(line, delimiters={}, grouping=None):
    start_delimiter = delimiters.keys()[0]
    end_delimiter = delimiters[start_delimiter]

    contents = extract_contents(line, {start_delimiter : end_delimiter})
    return smart_split(contents, grouping, True)

def get_key_contents(line, key):
    line = line.strip()
    if len(line) > 0 and line.startswith(key):
        return line[len(key):].strip()
    return ''

def get_key_args(line, key, grouping=None):
    contents = get_key_contents(line, key)
    return smart_split(contents, grouping, True)

def replace_args(line, delimiters, replacement, outer=False):
    start_delimiter = delimiters.keys()[0]
    end_delimiter = delimiters[start_delimiter]

    start, end = locate_delimiters(line, {start_delimiter : end_delimiter}, outer)
    if start != -1 and end != -1:
        return line[0:start] + str(replacement) + line[end + 1:]
    return line

def smart_split(line, grouping=None, keep_grouping_chars = False, split_char = " "):
    if grouping == None:
        grouping = { '"' : '"' }
    grouping_mode = False
    group_starter = None
    group_ender = None
    line_segments = []
    line_segment = ""
    for char in line:
        if grouping_mode:
            if char == group_ender:
                if keep_grouping_chars:
                    line_segment += char
                if len(line_segment) > 0:
                    line_segments.append(line_segment)
                line_segment = ""
                grouping_mode = False
            else:
                line_segment += char
            continue
        if char in grouping.keys():
            if len(line_segment) > 0:
                line_segments.append(line_segment)
            grouping_mode = True
            group_starter = char
            group_ender = grouping[char]
            line_segment = ""
            if keep_grouping_chars:
                line_segment += char
            continue
        if char == split_char:
            if len(line_segment) > 0:
                line_segments.append(line_segment)
            line_segment = ""
            continue
        line_segment += char
    if grouping_mode:
        raise ValueError("utils.smart_split(): unbalanced grouping of '" + group_starter + " " + group_ender + "' in '" + line + "'")
    if len(line_segment) > 0:
        line_segments.append(line_segment)
    return line_segments

def get_list_width(list):
  result = -1
  for item in list:
    length = len(str(item))
    if length > result:
      result = length
  return result

def sort_script(script_lines):
    script_lines.sort(reverse=False)

def is_number(str):
    try:
        int(str)
        return True
    except ValueError:
        return False

def reverse_find(subject, term):
    index = subject[::-1].find(term)
    return len(subject) - index - 1

# from https://stackoverflow.com/questions/2257441/random-string-generation-with-upper-case-letters-and-digits
def id_generator(size=6, chars=string.ascii_uppercase + string.digits):
   return ''.join(random.choice(chars) for _ in range(size))

def hash_object(object, brief=True, with_dashes=True):
    md5 = hashlib.md5()
    md5.update(str(object))
    hash = md5.hexdigest()
    if brief:
        hash = hash[1:8]
    elif with_dashes:
        hash = hash[1:8] + "-" + hash[8:16] + "-" + hash[16:24] + "-" + hash[24:32]
    return hash

def get_host_name():
  return socket.gethostname()

def get_ip_address(ifname='apcli0'):
  try:
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    return socket.inet_ntoa(fcntl.ioctl(
      s.fileno(),
      0x8915,  # SIOCGIFADDR
      struct.pack('256s', ifname[:15])
    )[20:24])
  except:
    return ""


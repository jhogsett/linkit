
import os
from subprocess import call
import sys
import time
import random
#import select
#import tty
#import termios

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

def randomize(seed=0, max=65535):
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
  filename, extension = os.path.splitext(filename)
  return extension

def get_filename_only(file_path):
  filename, extension = os.path.splitext(file_path)
  return filename

def get_path(file_path):
  return os.path.dirname(os.path.abspath(file_path))

# locate the start and end positions of a delimited portion of a string
# returns start, end
def locate_delimiters(line, start_delimiter, end_delimiter, outer=False):
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
def extract_contents(line, start_delimiter, end_delimiter, outer=False):
    line = line.strip()
    if len(line) == 0:
        return ''
    start, end = locate_delimiters(line, start_delimiter, end_delimiter, outer)
    if start != -1 and end != -1:
        return _cut_contents(line, start_delimiter, end_delimiter, start, end)
    return ''

# pass in line and two delimiters, get back list of arguments within
# delimiters specified as one or two characters
def extract_args(line, start_delimiter, end_delimiter):
    return extract_contents(line, start_delimiter, end_delimiter).split()

def get_key_contents(line, key):
    line = line.strip()
    if len(line) > 0 and line.startswith(key):
        return line[len(key):].strip()
    return ''

def get_key_args(line, key):
    return get_key_contents(line, key).split()

def replace_args(line, start_delimiter, end_delimiter, replacement, outer=False):
    start, end = locate_delimiters(line, start_delimiter, end_delimiter, outer)
    if start != -1 and end != -1:
        return line[0:start] + str(replacement) + line[end + 1:]
    return line

def smart_split(input_str, grouping_dict = {'"':'"'}, keep_grouping_chars=False, split_str=" "):
    final_split = []
    capture_mode = False
    capture_starter = None
    capture_ender = None
    capture_builder = []

    parts = input_str.split(split_str)
    num_parts = len(parts)

    grouping_starts = grouping_dict.keys()
    grouping_ends = grouping_dict.values()
    num_grouping_keys = len(grouping_starts)

    for p in range(0, num_parts):
        part = parts[p]

        if num_grouping_keys > 0:
            if capture_mode:
                if part.endswith(capture_ender):
                    capture_mode = False
                    capture_builder.append(part)
                    captured = split_str.join(capture_builder)
                    capture_builder = []
                    if keep_grouping_chars == False:
                        group_starter_size = len(capture_starter)
                        group_ender_size = len(capture_ender)
                        captured = captured[group_starter_size:-group_ender_size]
                    final_split.append(captured)
                else:
                    capture_builder.append(part)
                continue

            for i in range(0, num_grouping_keys):
                capture_starter = grouping_starts[i]
                if part.startswith(capture_starter):
                    capture_ender = grouping_ends[i];
                    if not part.endswith(capture_ender):
                        capture_mode = True
                        capture_ender = grouping_ends[i];
                        capture_builder.append(part)
                    break

            if capture_mode:
                continue

        final_split.append(part)

    if len(capture_builder) > 0:
        raise ValueError("utils.smart_split() unable to parse due to unbalanced grouping: " + input_str)

    return final_split



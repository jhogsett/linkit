
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
      new_lines.append(parts[0].strip())
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






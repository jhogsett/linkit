


import os
from subprocess import call

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

def get_pair(line, joiner="-"):
  line = line.strip()
  args = line.split(joiner)
  if len(args) == 0:
    return [None, None]
  elif len(args) == 1:
    return [args[0].strip(), None]
  elif len(args) == 2:
    return [args[0].strip(), args[1].strip()]

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


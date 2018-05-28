#!/usr/bin/python

# todo
# any number of spaces/tabs in variable setting

global macros, macro_commands, resolved, unresolved, passes, next_available_macro_number
macros = {}
macro_commands = {}
resolved = {}
unresolved = {}
passes = 0

# macros 10-13 are reserved, 0-9 are memory
starting_macro_number = 14
ending_macro_number = 51
next_available_macro_number = starting_macro_number

def set_macro(name, value):
  global macros
  macros[name] = value

def set_resolved(name, value):
  global resolved
  resolved[name] = value

def set_unresolved(name, value=None):
  global unresolved
  unresolved[name] = value

def resolve_unresolved(name, value=None):
  set_unresolved(name, value)

def remove_resolved():
  global unresolved
  new_dict = {}
  for name in unresolved:
    if unresolved[name] == None:
      new_dict[name] = None
  unresolved = new_dict

def unresolved_exist():
  return len(unresolved) > 0

def reset():
  global macros, macro_commands, resolved, unresolved, passes, next_available_macro_number
  macros = {}
  macro_commands = {}
  resolved = {}
  unresolved = {}
  passes = 0
  next_available_macro_number = starting_macro_number

def process_comment(line):
  line = line.strip()
  if len(line) > 0 and line[0] == "#":
    return ''
  return line

def process_blank_line(line):
# need to switch out of modes here
  line = line.strip()
  if len(line) == 0:
    return ''
  return line

def process_set_macro(line):
  macro_name = None
  macro_number = None
  line = line.strip()
  if len(line) > 0 and line[0] == "[" and line[-1] == "]":
    macro_name = line.strip(' []')
    args = macro_name.split()
    if len(args) == 2:
      macro_name, macro_number = args
    if macro_number == None:
      set_unresolved(macro_name)
      return "<" + macro_name + ">:set"
    else:
      set_resolved(macro_name, macro_number)
      set_macro(macro_name, macro_number)
      return str(macro_number) + ":set"
  return line

def process_macro_call(line):
  macro_name = None
  line_ = line.strip()
  if len(line) > 0 and line_[0] == "(" and line_[-1] == ")":
    macro_name = line_.strip(' ()')
    if macro_name in resolved:
      return str(resolved[macro_name]) + ":run"
  return line

def process_set_variable(line):
  line = line.strip()
  if len(line) > 0 and line[0] == "$":
    variable_value = None
    variable_name = line[1:]
    args = variable_name.split()
    if len(args) >= 2:
      variable_name, variable_value = args
    if variable_value == None:
      set_unresolved(variable_name)
    else:
      set_resolved(variable_name, variable_value)
    return ''
  return line

def process_get_variable(line):
  if len(line) > 0 and "<" in line and ">" in line:
    start_position = line.find("<")
    end_position = line.find(">")
    variable_name = line[start_position + 1:end_position].strip()
    if variable_name in resolved:
      resolved_value = resolved[variable_name]
      return line[0:start_position] + str(resolved_value) + line[end_position + 1:]
  return line

def process_line(line):
  line = process_blank_line(line)
  line = process_comment(line)
  line = process_set_variable(line)
  line = process_set_macro(line)
  line = process_macro_call(line)
  line = process_get_variable(line)
  return line

def is_macro_number_in_use(macro_number):
  for macro_name in macros:
    if macros[macro_name] == macro_number:
      return True
  return False

def get_next_macro_number():
  global next_available_macro_number
  while next_available_macro_number <= ending_macro_number:
    if not is_macro_number_in_use(next_available_macro_number):
      return next_available_macro_number
    else:
      next_available_macro_number += 1
  if next_available_macro_number > ending_macro_number:
    raise ValueError("No available macro numbers available")

def resolution_pass(script_lines):
  global passes
  new_lines = []
  for line in script_lines:
    new_line = process_line(line)
    if new_line != None:
      new_lines.append(new_line)
  passes += 1
  return filter(None, new_lines)

def resolve_macro_numbers():
  for name in unresolved:
    if unresolved[name] == None:
      new_macro_number = get_next_macro_number()
      resolve_unresolved(name, new_macro_number)
      set_resolved(name, new_macro_number)
      set_macro(name, new_macro_number)
  remove_resolved()

def resolve_script(script_lines):
  new_lines = resolution_pass(script_lines)
  resolve_macro_numbers()
  while True:
    prev_lines = new_lines
    new_lines = resolution_pass(new_lines)
    if new_lines == prev_lines:
      # no more resolving needed/possible
      break
  return new_lines

def consolidate_macros(script_lines):
  new_lines = []
  building_commands = ""
  for line in script_lines:
    if line.endswith(":set"):
      if len(building_commands) > 0:
        new_lines.append(building_commands[:-1])
        building_commands = ""
    building_commands += line + ":"
  if len(building_commands) > 0:
    new_lines.append(building_commands[:-1])
  return new_lines

def sort_script(script_lines):
  script_lines.sort()

# ----------------

def compile_script(script):
  new_script = resolve_script(script)
  new_lines = consolidate_macros(new_script)
  sort_script(new_lines)
  return new_lines


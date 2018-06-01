#!/usr/bin/python

import os

global macros, macro_commands, resolved, unresolved, passes, next_available_macro_number, next_available_sequencer_number, verbose_mode, starting_macro_number, ending_macro_number, presets, number_of_sequencers
macros = {}
macro_commands = {}
resolved = {}
unresolved = {}
passes = 0
verbose_mode = False
presets = {}

# macros 10-13 are reserved for apps, 0-9 are memory
starting_macro_number = 14
ending_macro_number = 50
next_available_macro_number = starting_macro_number
next_available_sequencer_number = 0
number_of_sequencers = 10

# ----------------------------------------------------

def begin(verbose_mode_ = False, presets_ = {}, starting_macro = 14, ending_macro = 50, number_of_sequencers_ = 10):
  global verbose_mode, starting_macro_number, ending_macro_number, presets, number_of_sequencers
  verbose_mode = verbose_mode_
  starting_macro_number = starting_macro
  ending_macro_number = ending_macro
  number_of_sequencers = number_of_sequencers_
  presets = presets_
  resolve_presets(presets)

def resolve_presets(presets):
  for key in presets.keys():
    set_resolved(key, presets[key])

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

def get_unresolved():
  return unresolved

def get_resolved():
  return resolved

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
  global macros, macro_commands, resolved, unresolved, passes, next_available_macro_number, next_available_sequencer_number
  macros = {}
  macro_commands = {}
  resolved = {}
  unresolved = {}
  passes = 0
  next_available_macro_number = starting_macro_number
  next_available_sequencer_number = 0
  resolve_presets(presets)

# ----------------------------------------------------

def process_comment(line):
  line = line.strip()
  if len(line) > 0 and line[0] == "#":
    return ''
  return line

def process_blank_line(line):
  line = line.strip()
  if len(line) == 0:
    return ''
  return line

def process_set_macro(line):
  macro_name = None
  macro_number = None
  line = line.strip()
  args = extract_args(line, "[", "]")
  if len(args) > 0:
    macro_name = args[0]
    if len(args) > 1:
      macro_number = args[1]
    if macro_number == None:
      set_unresolved(macro_name)
      return "<" + macro_name + ">:set"
    else:
      if macro_number == "!":
        macro_number = ending_macro_number
      set_resolved(macro_name, macro_number)
      set_macro(macro_name, macro_number)
      return str(macro_number) + ":set"
  return line

# todo: a way to pass args to run call
def process_macro_call(line):
  macro_name = None
  line = line.strip()
  macro_name = extract_contents(line, "(", ")")
  if macro_name in resolved:
    return str(resolved[macro_name]) + ":run"
  return line

def immutable_resolved_value_check(variable_name, variable_value):
  if variable_name in resolved:
    if resolved[variable_name] != variable_value:
      raise ValueError("The resolved value '%s' is being changed to '%s'" % (variable_name, str(variable_value)))

def process_set_variable(line):
  line = line.strip()
  if len(line) > 0 and not line_has_unresolved(line):
    args = get_key_args(line, "$")
    if len(args) >= 2:
      variable_name = args[0]
      # instead of taking arg #2, set the variable to the remainder of the line, so it can include spaces
      variable_value = line[len(variable_name) + 1:].strip()
      immutable_resolved_value_check(variable_name, variable_value)
      set_resolved(variable_name, variable_value)
      return ''
  return line

def process_get_variable(line):
  if len(line) < 1:
    return line
  args = extract_args(line, "<", ">")
  if len(args) > 0:
    variable_name = args[0]
    if variable_name in resolved:
      resolved_value = resolved[variable_name]
      return replace_args(line, "<", ">", resolved_value)
  return line

def process_allocate_sequencer(line):
  global next_available_sequencer_number
  if len(line) < 1:
    return line
  args = extract_args(line, "{", "}")
  if len(args) > 0:
    sequencer_name = args[0]
    resolnext_available_sequencer_numberved_value = None
    if sequencer_name in resolved:
      resolved_value = resolved[sequencer_name]
    else:
      if next_available_sequencer_number > (number_of_sequencers - 1):
        raise ValueError("No available sequence numbers available")
      resolved_value = next_available_sequencer_number
      set_resolved(sequencer_name, resolved_value)
      next_available_sequencer_number += 1
      return replace_args(line, "{", "}", str(resolved_value))
  return line

def process_evaluate_python(line):
  if len(line) < 1:
    return line
  if not line_has_unresolved_variables(line):
    expression = extract_contents(line, "`", "`")
    if len(expression) > 0:
      return replace_args(line, "`", "`", str(eval(expression)))
  return line

def process_place_template(line):
  if len(line) < 1:
    return line
  args = extract_args(line, "((", "))")
  if len(args) > 0:
    variable_name = args[0]
    if variable_name in resolved:
      template_script = resolved[variable_name]
      return replace_args(line, "<>", resolved_value)
  return line  

def process_line(line):
  line = process_blank_line(line)
  line = process_comment(line)
  line = process_evaluate_python(line)
  line = process_set_variable(line)
  line = process_set_macro(line)
  line = process_macro_call(line)
  line = process_get_variable(line)
  line = process_allocate_sequencer(line)
  line = process_place_template(line)
  return line

def line_has_unresolved_variables(line):
  return len(line) > 0 and "<" in line and ">" in line

def line_has_python_expression(line):
  return len(line) > 0 and "`" in line

def line_has_template_marker(line):
  return len(line) > 0 and ("[[" in line or "]]" in line or "((" in line or "))" in line)

def line_has_unresolved(line):
  return line_has_unresolved_variables(line) or line_has_python_expression(line) or line_has_template_marker(line)

# ----------------------------------------------------

# locate the start and end positions of a delimited portion of a string
# returns start, end
def locate_delimiters(line, start_delimiter, end_delimiter):
  start = -1
  end = -1
  if start_delimiter in line:
    start = line.find(start_delimiter)
    if end_delimiter in line[start + len(start_delimiter):]:
      end = line.find(end_delimiter, start + 1)
  return start, end

def cut_contents(line, start_delimiter, end_delimiter, start, end):
  return line[start + len(start_delimiter):end].strip()

# pass in line and two delimiters, get back contents within
# delimiters specified as one or two characters
def extract_contents(line, start_delimiter, end_delimiter):
  line = line.strip()
  if len(line) == 0:
    return ''
  start, end = locate_delimiters(line, start_delimiter, end_delimiter)
  if start != -1 and end != -1:
    return cut_contents(line, start_delimiter, end_delimiter, start, end)
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

def replace_args(line, start_delimiter, end_delimiter, replacement):
  start, end = locate_delimiters(line, start_delimiter, end_delimiter)
  if start != -1 and end != -1:
    return line[0:start] + str(replacement) + line[end + 1:]
  return line

# ----------------------------------------------------

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

  new_lines = filter(None, new_lines)

  if verbose_mode:
    print
    print "pass# %d" % passes
    print_script(new_lines)
    print
    print "resolved:"
    print_list(resolved)

  return new_lines

def resolve_macro_numbers():
  for name in unresolved:
    if unresolved[name] == None:
      new_macro_number = get_next_macro_number()
      resolve_unresolved(name, new_macro_number)

# at this point, it could program the macro to determine how many bytes are used
# if the # of bytes are exceeded, then mark the following macro as excess space in some way
# then the macro has enough space to run using two macro slots

      set_resolved(name, new_macro_number)
      set_macro(name, new_macro_number)
  remove_resolved()

def resolve_script(script_lines):
  if verbose_mode:
    print "--------------------------------------------"
  new_lines = capture_templates(script_lines)
  new_lines = expand_templates(new_lines)
  if verbose_mode:
    print "Expanded script:"
    print_script(new_lines)
    print
  new_lines = resolution_pass(new_lines)
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

def capture_templates(script_lines):
  new_lines = []
  template_builder = []
  capture_mode = False
  template_name = None
  for line in script_lines:
    line = line.strip()
    if capture_mode:
      if line.startswith("]]"):
        capture_mode = False
        set_resolved(template_name, template_builder)
        template_builder = []
      else:
        template_builder.append(line)
    else:
      if "[[" in line:
        args = get_key_args(line, "[[")
        template_name = args[0]
        combined_args = " ".join(args[1:])
        # store the search strings that will be replaced with passed arguments later
        template_builder.append(combined_args)
        capture_mode = True
      else:
        new_lines.append(line)
  return new_lines

def template_replacements(template_lines, keys, replacements):
  new_lines = []
  for line in template_lines:
    for index, key in enumerate(keys):
      line = line.replace(key, replacements[index])
    new_lines.append(line)
  return new_lines

def expand_templates(script_lines):
  new_lines = []
  for line in script_lines:
    line = line.strip()
    args = extract_args(line, "((", "))")
    if len(args) > 0:
      template_name = args[0]
      # remaining arguments, if any, are the search replacements
      replacements = args[1:]
      if template_name in resolved:
        template_script = resolved[template_name]
        # first line is the search keys
        keys = template_script[0].split()
        # remaining lines are the template contents
        template_script = template_script[1:]
        template_script = template_replacements(template_script, keys, replacements)
        new_lines = new_lines + template_script
    else:
      new_lines.append(line)
  return new_lines

# ----------------------------------------------------

def load_file(filename, default_ext=".mac"):
  file_lines = []
  if not filename.endswith(default_ext):
    filename = filename + default_ext
  file_path = os.path.dirname(filename)
  file = open(filename, "r")
  for line in file:
    line = line.strip()
    if len(line) == 0:
      continue
    if line.startswith("%include"):
      args = line.split()
      include_filename = args[1]
      if len(include_filename) > 0:
        include_filename = os.path.join(file_path, include_filename)
        file_lines = file_lines + load_file(include_filename)
        continue
    file_lines.append(line)
  return file_lines

# ----------------------------------------------------

def compile_script(script):
  new_script = resolve_script(script)
  new_lines = consolidate_macros(new_script)
  sort_script(new_lines)
  return new_lines

def compile_file(filename):
  script = load_file(filename)
  return compile_script(script)

def compilation_valid(script):
  for line in script:
    if line_has_unresolved(line):
      return False
  return True

def print_script(script):
  for line in script:
    print line
  print

def print_list(list):
  for key in list.keys():
    print str(key) + "='" + str(list[key]) + "'"


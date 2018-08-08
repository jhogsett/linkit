#!/usr/bin/python

import os
import app_ui as ui
import terminal_colors as tc
import long_commands as lc
import code

global macros, macro_commands, resolved, unresolved, passes, next_available_macro_number, next_available_sequencer_number, verbose_mode, starting_macro_number, ending_macro_number, presets, number_of_sequencers
global number_of_macros, led_command, final_macro_numbers, saved_bad_script, includes, last_macro_bytes
macros = {}
macro_commands = {}
resolved = {}
unresolved = {}
passes = 0
verbose_mode = False
presets = {}
final_macro_numbers = {}
starting_macro_number = None
ending_macro_number = None
next_available_macro_number = None
next_available_sequencer_number = 0
number_of_sequencers = None
number_of_macros = None
bytes_per_macro = None
led_command = None
saved_bad_script = []
includes = {}
last_macro_bytes = None

# ----------------------------------------------------

def begin(led_command_, verbose_mode_, presets_, starting_macro, ending_macro, number_of_sequencers_, bytes_per_macro_, max_string_length_, last_macro_bytes_):
  global verbose_mode, starting_macro_number, ending_macro_number, presets, number_of_sequencers, number_of_macros, led_command, bytes_per_macro, max_string_length, next_available_macro_number, last_macro_bytes
  led_command = led_command_
  verbose_mode = verbose_mode_
  starting_macro_number = starting_macro
  ending_macro_number = ending_macro
  number_of_sequencers = number_of_sequencers_
  bytes_per_macro = bytes_per_macro_
  max_string_length = max_string_length_
  number_of_macros = (ending_macro_number - starting_macro_number) + 1
  next_available_macro_number = starting_macro_number
  last_macro_bytes = last_macro_bytes_
  presets = presets_
  resolve_presets(presets)
  ui.begin(verbose_mode)
  ui.report_verbose("Beginning compilation engine")

def resolve_presets(presets):
  for key in presets.keys():
    ui.report_verbose("setting preset resolved value " + tc.yellow(key + "=" + str(presets[key])))
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

def get_final_macro_numbers():
  return final_macro_numbers

def get_saved_bad_script():
  return saved_bad_script

def get_presets():
  return presets

def get_macros():
  return macros

def get_includes():
  return includes

def reset():
  global macros, macro_commands, resolved, unresolved, passes, next_available_macro_number, next_available_sequencer_number, final_macro_numbers, includes
  macros = {}
  macro_commands = {}
  resolved = {}
  unresolved = {}
  final_macro_numbers = {}
  saved_bad_script = []
  includes = {}
  passes = 0
  next_available_macro_number = starting_macro_number
  next_available_sequencer_number = 0
  resolve_presets(presets)

def reset_next_available_sequence_number():
  next_available_sequencer_number = 0

# ----------------------------------------------------

def process_comment(line):
  line = line.strip()
  if len(line) > 0 and line[0] == "#":
    return ''
  if "#" in line:
    return line.split("#")[0]
  return line

def process_blank_line(line):
  line = line.strip()
  if len(line) == 0:
    return ''
  return line

def process_set_macro(line):

  if line_has_unresolved_variables(line):
    return line

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
      ui.report_verbose("new unresolved macro: " + macro_name)
      return "<" + macro_name + ">:set"
    else:
      if macro_number == "!":
        ui.report_verbose("- forced final macro: " + macro_name)
        macro_number = ending_macro_number
#@@@      proxy_macro_number = "'" + ("0" + str(macro_number))[-2:] + "'"
      proxy_macro_number = str(macro_number)

      final_macro_numbers[macro_number] = int(macro_number)

      ui.report_verbose("new forced macro: " + macro_name)
      set_resolved(macro_name, proxy_macro_number)
      set_macro(macro_name, proxy_macro_number)

      # setting a specific macro number is done for apps
      # to combine apps, the next available sequence number
      # needs to be reset. 
      # if a macro number is passed, assume it's an app
      # and do the resetting (unless it's the last)

      #needed????????
      if macro_number != ending_macro_number:
        reset_next_available_sequence_number()

      #return str(proxy_macro_number) + ":set"
      return "'" + str(int(macro_number) * -1) + "':set"
  return line

def process_macro_call(line):
  macro_name = None
  line = line.strip()
  args = extract_args(line, "(", ")")
  if len(args) > 0:
    macro_name = args[0]
    if macro_name in resolved:
      if len(args) > 1:
        macro_schedule = args[1]
        if not line_has_unresolved(macro_schedule):
          return str(macro_schedule) + "," + str(resolved[macro_name]) + ":sch"

#also try without conditional

      else:
        return str(resolved[macro_name]) + ":run"
  return line

# return true if value is immutable and is a preset being overridden
# raise ValueError if value is immutable and is not a preset
def immutable_resolved_value(variable_name, variable_value):
  if variable_name in resolved:
    if resolved[variable_name] != variable_value:
      if variable_name in presets:
        return True 
      else:
        raise ValueError("The immutable resolved value '%s' is being changed to '%s'" % (variable_name, str(variable_value)))
  return False

def process_set_variable(line):
  line = line.strip()
  if len(line) > 0 and not line_has_unresolved(line):
    args = get_key_args(line, "$")
    if len(args) >= 2:
      variable_name = args[0]
      # instead of taking arg #2, set the variable to the remainder of the line, so it can include spaces
      variable_value = line[len(variable_name) + 1:].strip()
      if not immutable_resolved_value(variable_name, variable_value):
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
    resolved_value = None
    if sequencer_name in resolved:
      resolved_value = resolved[sequencer_name]
    else:
      if next_available_sequencer_number > (number_of_sequencers - 1):
        raise ValueError("No available sequence numbers available")
      resolved_value = next_available_sequencer_number
      ui.report_verbose("allocating sequencer: " + str(resolved_value))
      set_resolved(sequencer_name, resolved_value)
      next_available_sequencer_number += 1
    return replace_args(line, "{", "}", str(resolved_value))
  return line

def process_evaluate_python(line):
  if len(line) < 1:
    return line
  expression = extract_contents(line, "`", "`")
  if not line_has_unresolved_variables(expression):
    if len(expression) > 0:
      ui.report_verbose_alt("-evaluating Python: " + expression)
      result = eval(expression)
      ui.report_verbose_alt("=evaluated result: " + str(result))
      return replace_args(line, "`", "`", str(result))
  return line

def process_place_template(line):
  if len(line) < 1:
    return line
  args = extract_args(line, "((", "))")
  if len(args) > 0:
    template_name = args[0]
    if template_name in resolved:
      template_script = resolved[template_name]
      ui.report_verbose("-placing template: " + template_name)
      return replace_args(line, "((", "))", template_script)
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

def line_has_macro_marker(line):
  return len(line) > 0 and ("(" in line or ")" in line or "[" in line or "]" in line)

def line_has_template_marker(line):
  return len(line) > 0 and ("[[" in line or "]]" in line or "((" in line or "))" in line)

def line_has_sequence_marker(line):
  return len(line) > 0 and ("{" in line or "}" in line)

def line_has_unresolved(line):
  return line_has_unresolved_variables(line) or line_has_python_expression(line) or line_has_template_marker(line) or line_has_macro_marker(line) or line_has_sequence_marker(line)

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

def report_progress():
    if not verbose_mode:
        ui.write(tc.green("."))

# expects a proxy macro number in the form '4'
def is_macro_number_in_use(macro_number):
  for value in macros.values():
    if "'" in str(value):
      value = int(value[1:-1])
    if value == macro_number:
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
  if verbose_mode:
    ui.report_verbose("Resolution pass #" + str(passes))
  report_progress()
  new_lines = filter(None, new_lines)
  return new_lines

# assign tentative macro numbers so everything else can resolve
# these will be resolved to real macro numbers later
def proxy_macro_numbers():
  ui.report_verbose("unresolved macro numbers:")
  if verbose_mode:
    print_list(unresolved)
  for name in unresolved:
    if unresolved[name] == None:
      new_macro_number = get_next_macro_number()
      # proxy numbers will be in the form '10' 
      ui.report_verbose("-assigning proxy macro #" + str(new_macro_number) + " for macro: " + name)
      proxy_macro_value = "'" + ("0" + str(new_macro_number))[-2:] + "'"
      resolve_unresolved(name, new_macro_number)
      set_resolved(name, proxy_macro_value)
      set_macro(name, proxy_macro_value)
  remove_resolved()

def assign_final_macro_number(line):
  global final_macro_numbers, saved_bad_script
  start, end = locate_delimiters(line, "'", "'")
  # only process lines starting with proxy macro numbers
  if start != 0:
    return line
  proxy_macro_number = int(extract_contents(line, "'", "'"))
  # temporarily replace this macro's unresolved references 
  # with a memory macro to use to measure the size
  # use macro #0 to have the most available space
  test_macro = replace_args(line, "'", "'", "0")
  # replace remaining references with #1 to ensure args are stored
  while "'" in test_macro:
    test_macro = replace_args(test_macro, "'", "'", "1")
  # send to the device and check for consumed macro bytes
  if len(test_macro) > max_string_length:
    raise ValueError("Macro being tested exceeds char buffer size")
    # todo: handle this automatically
  bytes_used = 0
  tries = 3
  led_command.stop_all()
  while bytes_used == 0 and tries > 0:
    ui.report_verbose("Measuring proxy macro #" + str(proxy_macro_number) + " on device")
    bytes_used = led_command.command_int(test_macro)
    ui.report_verbose("-reported size: " + str(bytes_used) + " bytes")
    tries -= 1
  if bytes_used == 0:
    # todo: need more appropriate error type
    raise ValueError("Macro size measurement failed with retries")
  usable_bytes_per_macro = bytes_per_macro - 1 
  macro_slots_required = bytes_used / usable_bytes_per_macro
  if bytes_used % usable_bytes_per_macro != 0:
    macro_slots_required += 1
  final_macro_number = None
  # handle fixed macro numbers
  if proxy_macro_number < 0:
    proxy_macro_number *= -1
    for x in range(1, macro_slots_required):
      try_macro_number = proxy_macro_number + x
      if try_macro_number in final_macro_numbers.values():
        saved_bad_script = [line]
        raise ValueError("No block of macros for fixed macro during final number assignment")
    final_macro_number = proxy_macro_number
  else:
    potential_macro_number = starting_macro_number
    last_potential_macro_number = ending_macro_number - (macro_slots_required - 1)
    retry = False
    while potential_macro_number <= last_potential_macro_number:
      #ui.report_verbose("trying macro number: " + str(potential_macro_number))
      for x in range(0, macro_slots_required):
        try_macro_number = potential_macro_number + x
        if try_macro_number in final_macro_numbers.values():
          #ui.report_verbose("macro #%d already in use" % try_macro_number)
          retry = True
          break
      if retry:
        retry = False
        potential_macro_number += 1
        #ui.report_verbose("trying next macro #%d" % potential_macro_number)
        if potential_macro_number > ending_macro_number:
          saved_bad_script = [line]
          raise ValueError("No available macro numbers available during final number assignment")
        continue
      break
    # if the last position was chosen, check that there are enough bytes in the last macro
    final_macro_number = potential_macro_number
  if verbose_mode:
    ui.report_verbose("-assigning final macro #" + str(final_macro_number) + " for proxy #" + str(proxy_macro_number))
  report_progress()
  final_macro_numbers[proxy_macro_number] = final_macro_number

  # FIX
  if final_macro_number == ending_macro_number and bytes_used > last_macro_bytes:
    raise ValueError("Not enough remaining bytes in the last macro position for Macro %d" % final_macro_number)

  # consume any additional macro numbers to account for byte overage
  consumed_macro_number = final_macro_number
  remaining_bytes = bytes_used - (bytes_per_macro-1)
  while remaining_bytes > 0:
    consumed_macro_number += 1
    remaining_bytes -= (bytes_per_macro-1)
    # create a unique key to hold the additional consumed macro number value
    if consumed_macro_number in final_macro_numbers.values():
      saved_bad_script = [line]
      raise ValueError("Macro %d is needed as a carry-over macro but is already assigned" % consumed_macro_number)
    ui.report_verbose("-allocating macro #" + str(consumed_macro_number) + " to macro #" + str(proxy_macro_number)) 
    final_macro_numbers[str(proxy_macro_number) + "-" + str(consumed_macro_number)] = consumed_macro_number
  # return the line with the proxy macro number replaced so it's not processed a second time
  return replace_args(line, "'", "'", str(final_macro_number)) 

def process_finalized_macro_numbers_pass(script_lines):
  new_lines = []
  for line in script_lines:
    args = extract_args(line, "'", "'")
    if len(args) == 1:
      proxy_macro_number = int(args[0])
      if proxy_macro_number in final_macro_numbers:
        final_macro_number = final_macro_numbers[proxy_macro_number]
        new_line = replace_args(line, "'", "'", final_macro_number)
        new_lines.append(new_line)
      else:
        new_lines.append(line)
    else:
      new_lines.append(line)
  return new_lines

def process_finalized_macro_numbers(script_lines):
  processed_lines = script_lines
  while True:
    prev_lines = processed_lines
    if verbose_mode:
      ui.report_verbose("-processing pass to finalize macro numbers")
    report_progress()
    processed_lines = process_finalized_macro_numbers_pass(processed_lines)
    if processed_lines == prev_lines:
      # no more resolving is possible
      return processed_lines

# first pass - assign final macro numbers, measure programmed size,
# and allocate any additional macro slots needed for byte overage
def assign_final_macro_numbers_pass_one(script_lines):
  new_lines = []
  for line in script_lines:
    new_lines.append(assign_final_macro_number(line))
  return new_lines

# second pass - replace proxy macro numbers with final numbers
def assign_final_macro_numbers_pass_two(script_lines):
  new_lines = process_finalized_macro_numbers(script_lines)
  return new_lines

def assign_final_macro_numbers(script_lines):
  processed_lines = script_lines
  while True:
    prev_lines = processed_lines
    processed_lines = assign_final_macro_numbers_pass_one(processed_lines)
    processed_lines = assign_final_macro_numbers_pass_two(processed_lines)
    if processed_lines == prev_lines:
      # no more processing is possible
      return processed_lines

########################################################################
# Main compilation engine
########################################################################

def resolve_script(script_lines):

  ########################################################################
  # pre-processing - template capturing, template expansion
  ########################################################################

  ui.report_verbose("Pre-processing")

  new_lines = remove_blank_lines(script_lines)
  new_lines = remove_comments(new_lines)
  new_lines = pre_rewrite(new_lines)
  new_lines = translate_commands(new_lines)
  new_lines = process_directives(new_lines)
  new_lines = capture_templates(new_lines)
  new_lines = expand_meta_templates(new_lines)
  new_lines = expand_templates(new_lines)

  if verbose_mode:
    ui.report_verbose("Expanded script:")
    print_script(new_lines)
    ui.report_verbose()

  ########################################################################
  # initial processing - assign tentative macro numbers 
  ########################################################################

  ui.report_verbose("Initial processing")
  new_lines = resolution_pass(new_lines)

  ui.report_verbose("script after initial resolution pass:")
  if verbose_mode:
    print_script(new_lines)

  proxy_macro_numbers()

  ui.report_verbose("script after proxy macro number assignment:")
  if verbose_mode:
    print_script(new_lines)

  ########################################################################
  # main processing - processing passes until no more can be resolved
  ########################################################################

  ui.report_verbose("Main processing")
  while True:
    prev_lines = new_lines
    new_lines = resolution_pass(new_lines)
    if new_lines == prev_lines:
      # no more resolving needed/possible
      break
  return new_lines

# this assumes all commands are on individual lines
def translate_commands(script_lines):
  new_lines = []
  for line in script_lines:
    line = line.strip()
    line = lc.translate(line)
    new_lines.append(line)
  ui.report_verbose("script after command translation:")
  if verbose_mode:
      print_script(new_lines)
  return new_lines

def post_processing(script_lines):
  script_lines.sort()
  return assign_final_macro_numbers(script_lines)

def do_clean_ups(script_lines, clean_ups):
    new_lines = []
    for line in script_lines:
        for clean_up in clean_ups:
            line = line.replace(clean_up, clean_ups[clean_up])
        new_lines.append(line)
    return new_lines

# rewrite the script in the new style without the colons
# this simplifies automatic modifying of the script
def pre_rewrite(script_lines):
    new_lines = []
    for line in script_lines:
        segments = line.split(":")
        for segment in segments:
            new_lines.append(segment)
    ui.report_verbose("script after pre-rewrite:")
    if verbose_mode:
        print_script(new_lines)
    return new_lines

def post_clean_up(script_lines):
    clean_ups = {
        # remove unnecessary (zero) arguments
        ",0:" : ":",
        ",0,0:" : ":",
        ":0,0,0:" : ":"
    }
    return do_clean_ups(script_lines, clean_ups)

def process_directives(script_lines):
  new_lines = []
  for line in script_lines:
    args = get_key_args(line, "%")
    if len(args) >= 2:
      directive_name = "%" + args[0]
      if len(args) == 2:
        directive_value = args[1]
      elif len(args) == 3:
        directive_value = { args[2] : args[3] }
      else:
        directive_value = args[1:].split()
      set_resolved(directive_name, directive_value)
    else:
      new_lines.append(line)
  return new_lines

def remove_blank_lines(script_lines):
  new_lines = []
  for line in script_lines:
    line = process_blank_line(line)
    if len(line) > 0:
      new_lines.append(line)
  return new_lines

def remove_comments(script_lines):
  new_lines = []
  for line in script_lines:
    line = process_comment(line)
    if len(line) > 0:
      new_lines.append(line)
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
  script_lines.sort(reverse=False)

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
  lines = template_lines
  while(True):
    orig_lines = lines
    new_lines = []
    for line in lines:
      for index, key in enumerate(keys):
        line = line.replace(key, replacements[index])
      new_lines.append(line)
    if new_lines == orig_lines:
      return new_lines
    lines = new_lines

def expand_templates(script_lines):
  new_lines = []
  for line in script_lines:
    line = line.strip()
    if "((" in line and "(((" not in line:
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

def expand_meta_templates(script_lines):
  new_lines = []
  for line in script_lines:
    line = line.strip()
    args = extract_args(line, "(((", ")))")
    if len(args) >= 2:
      template_name = args[0]
      index_arg = args[1]
      index_max = None
      start, end = locate_delimiters(index_arg, "`", "`")
      if start != -1 and end != -1:
        expression = extract_contents(index_arg, "`", "`")
        # replace any preset arguments to allow use in computing macro index max
        for key in presets:
          expression = expression.replace(key, str(presets[key]))
        index_max = eval(expression) 
      else:
        try:
          index_max = int(index_arg)
        except:
          raise ValueError("Meta template cannot be expanded due to non-integer argument: " + str(args[1]));
      # remaining arguments, if any, are the search replacements
      replacements = " ".join(args[2:])
      for index in range(0, index_max):
        new_line = "((" + template_name + " " + str(index) + " " + replacements + "))"
        new_lines.append(new_line)
    else:
      new_lines.append(line)
  return new_lines

# ----------------------------------------------------

def remove_fixed_macro_numbers(line):
  start, end = locate_delimiters(line, "[", "]")
  if start != -1 and end != -1:
    args = extract_args(line, "[", "]")
    # leave only the name
    return "[" + args[0] + "]"
  else:
    return line

def rewrite_included_script_line(script_line):
  new_line = remove_fixed_macro_numbers(script_line)
  return new_line

def rewrite_included_script_lines(script_lines):
  new_lines = []
  for line in script_lines:
    line = line.strip()
    new_lines.append(rewrite_included_script_line(line))
  return new_lines

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
      if len(include_filename) > 0 and include_filename not in includes.keys():
        full_filename = os.path.join(file_path, include_filename)
        include_lines = load_file(full_filename)
        include_lines = rewrite_included_script_lines(include_lines)
        file_lines = file_lines + include_lines
        includes[include_filename] = full_filename
        continue
    file_lines.append(line)
  return file_lines

# ----------------------------------------------------

def compile_script(script):
  global saved_bad_script
  ui.report_verbose("Compiling")
  new_script = resolve_script(script)
  new_lines = consolidate_macros(new_script)

  ui.report_verbose("consolidated script with proxied macro numbers")
  if verbose_mode:
    print_script(new_lines)

  sort_script(new_lines)

  ui.report_verbose("script after sorting")
  if verbose_mode:
    print_script(new_lines)

  #ui.report_verbose()
  if not compilation_valid(new_lines):
    saved_bad_script = new_lines
    raise ValueError("The script did not compile successfully due to unresolved values.")
  ui.report_verbose("Packing")
  #print_script(new_lines)

  new_lines = post_processing(new_lines)
  ui.report_verbose("script after final macro number assignment")
  if verbose_mode:
    print_script(new_lines)

  new_lines = post_clean_up(new_lines)
  ui.report_verbose("script after cleanup:")
  if verbose_mode:
    print_script(new_lines)

  return new_lines

def compile_file(filename):
  script = load_file(filename)
  return compile_script(script)

def compilation_valid(script):
  for line in script:
    if line_has_unresolved(line):
      return False
  return True

def remaining_sequencers():
  return number_of_sequencers - next_available_sequencer_number 

def print_script(script):
  for line in script:
    ui.report_error(line)
  print

def print_list(list):
  for key in list.keys():
    print str(key) + "='" + str(list[key]) + "'"


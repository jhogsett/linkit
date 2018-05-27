#!/usr/bin/python

global macros, resolved, unresolved
macros = {}
resolved = {}
unresolved = {}

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

def reset():
  global macros, resolved, unresolved
  macros = {}
  resolved = {}
  unresolved = {}

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

# ----------------

def expect(description, expected, got):
  if expected != got:
    print "failed: " + description

process_line(" [ lamp 12 ] ")
expect("setting forced macro name, number", resolved["lamp"], "12")

process_line("$setup 9")
expect("setting variable", resolved["setup"], "9")

new_line = process_line("(setup)")
expect("dereferencing macro call", new_line, "9:run")

process_line("[macro 15]")
new_line = process_line("(macro)")
expect("processing macro call line", new_line, "15:run")

new_line = process_line("(where)")
expect("no line touch on unresolved macro call", new_line, "(where)")

process_line("[render]")
expect("unresolved macro", "render" in unresolved, True)

process_line("$number 19")
new_line = process_line("red:<number>:rep")
expect("replaced variable ref", new_line, "red:19:rep")

reset()

script = '''
# test script

$max 10

[init]
red
<max>:rep
(render)

[doit 1]
blu

[render]
flu
'''

script_lines = script.split('\n')

print script

new_lines = []
for line in script_lines:
  new_line = process_line(line)
  if new_line != None:
    new_lines.append(new_line)
    print new_line

print

new_lines = filter(None, new_lines)

expected_script_lines = ['<init>:set', 'red', '10:rep', '(render)', '1:set', 'blu', '<render>:set', 'flu']
expect("first pass parsing a script", new_lines, expected_script_lines)

#print new_lines

#print "resolved: " + str(resolved)
#print "unresolved: " + str(unresolved)

global next_available_macro_number

starting_macro_number = 10
ending_macro_number = 51
next_available_macro_number = starting_macro_number

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
    


# make a pass at resolving
for name in unresolved:
  if unresolved[name] == None:
    new_macro_number = get_next_macro_number()
    resolve_unresolved(name, new_macro_number)
    set_resolved(name, new_macro_number)
    set_macro(name, new_macro_number)

print resolved
print unresolved
print macros

script_lines = new_lines
new_lines = []
for line in script_lines:
  new_line = process_line(line)
  if new_line != None:
    new_lines.append(new_line)
    print new_line

print

new_lines = filter(None, new_lines)

print new_lines
expected_script_lines = ['10:set', 'red', '10:rep', '11:run', '1:set', 'blu', '11:set', 'flu']
expect("second pass parsing a script", new_lines, expected_script_lines)




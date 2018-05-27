#!/usr/bin/python

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
  global macros, macro_commands, resolved, unresolved, passes
  macros = {}
  macro_commands = {}
  resolved = {}
  unresolved = {}
  passes = 0

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

# ----------------

def expect(description, expected, got):
  if expected != got:
    print "failed: " + description

def print_script(script):
  for line in script:
    print line
  print

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

script_text = '''
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

script = script_text.split('\n')
new_script = resolve_script(script)

expected_script = ['14:set', 'red', '10:rep', '15:run', '1:set', 'blu', '15:set', 'flu']
expect("compiled script #1", expected_script, new_script)

new_lines = consolidate_macros(new_script)
print_script(new_lines)

reset()

script_text = '''
# red green and blue wandering pixels with mirror

#[name {number}] start macro
#(name) run macro

# variables
$max 90
$max-half 45
$render-time 50
$sequence-time 20
$fade-type ffd
$red-seq 0
$green-seq 1
$blue-seq 2
$wander-range 3
$wander-neg-range -1

# use variables or recall macro numbers <name>

# command value substitutions
# these should be all caps built-in?
$get-current -1
$macro-seq -4


# fancy app
[fancy 10]
(init)
(setup-rendering)
(start-components)

# initialization
[init]
# these should be an app command, including shuffle reset
#-1:sch
#1:pau
#2:cnt
#era
#flu
app

# rendering
[setup-rendering]
<render-time>,<render>:sch

[render]
mir
flu

# start up components
[start-components]
(start-red)
(start-green)
(start-blue)

# red wanderer
[start-red]
<red-seq>,<max>,0:seq
<sequence-time>,<red-sequence>:sch

[red-sequence]
<red-seq>,<macro-seq>,<red-math>:seq
pos
<red-seq>,0,1:pal
<fade-type>
rst

[red-math]
<wander-range>:rng:psh
<wander-neg-range>:psh
add
<red-seq>,<get-current>:seq:psh
add
<max-half>:psh
mod

# green wanderer
[start-green]
<green-seq>,<max>,0:seq
<sequence-time>,<green-sequence>:sch

[green-sequence]
<green-seq>,<macro-seq>,<green-math>:seq
pos
<green-seq>,0,1:pal
<fade-type>
rst

[green-math]
<wander-range>:rng:psh
<wander-neg-range>:psh
add
<green-seq>,<get-current>:seq
psh
add
<max-half>:psh
mod


# blue wanderer
[start-blue]
<blue-seq>,<max>,0:seq
<sequence-time>,<blue-sequence>:sch

[blue-sequence]
<blue-seq>,<macro-seq>,<blue-math>:seq
pos
<blue-seq>,0,1:pal
<fade-type>
rst

[blue-math]
<wander-range>:rng:psh
<wander-neg-range>:psh
add
<blue-seq>,<get-current>:seq
psh
add
<max-half>:psh
mod
'''

script = script_text.split('\n')
new_script = resolve_script(script)
expected_script = ['10:set', '22:run', '23:run', '25:run', '22:set', 'app', '23:set', '50,17:sch', '17:set', 'mir', 'flu', '25:set', '27:run', '21:run', '20:run', '27:set', '0,90,0:seq', '20,18:sch', '18:set', '0,-4,16:seq', 'pos', '0,0,1:pal', 'ffd', 'rst', '16:set', '3:rng:psh', '-1:psh', 'add', '0,-1:seq:psh', 'add', '45:psh', 'mod', '21:set', '1,90,0:seq', '20,24:sch', '24:set', '1,-4,19:seq', 'pos', '1,0,1:pal', 'ffd', 'rst', '19:set', '3:rng:psh', '-1:psh', 'add', '1,-1:seq', 'psh', 'add', '45:psh', 'mod', '20:set', '2,90,0:seq', '20,15:sch', '15:set', '2,-4,26:seq', 'pos', '2,0,1:pal', 'ffd', 'rst', '26:set', '3:rng:psh', '-1:psh', 'add', '2,-1:seq', 'psh', 'add', '45:psh', 'mod']
expect("compiled script #2", expected_script, new_script)

new_lines = consolidate_macros(new_script)
print_script(new_lines)


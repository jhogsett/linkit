#!/usr/bin/python

global resolved, unresolved
resolved = {}
unresolved = {}

def set_resolved(name, value):
  resolved[name] = value

def set_unresolved(name):
  unresolved[name] = None

def process_set_macro(line):
  macro_name = None
  macro_number = None
  line = line.strip()
  if line[0] == "[" and line[-1] == "]":
    macro_name = line.strip(' []')
    args = macro_name.split()
    if len(args) == 2:
      macro_name, macro_number = args
    if macro_number == None:
      set_unresolved(macro_name)
    else:
      set_resolved(macro_name, macro_number)
    return True
  return False

def process_macro_call(line):
  macro_name = None
  line_ = line.strip()
  if line_[0] == "(" and line_[-1] == ")":
    macro_name = line_.strip(' ()')
    if macro_name in resolved:
      return str(resolved[macro_name]) + ":run"
  return line

def process_set_variable(line):
  line = line.strip()
  if line[0] == "$":
    variable_value = None
    variable_name = line[1:]
    args = variable_name.split()
    if len(args) >= 2:
      variable_name, variable_value = args
    if variable_value == None:
      set_unresolved(variable_name)
    else:
      set_resolved(variable_name, variable_value)
    return True
  return False

def process_get_variable(line):
  if "<" in line and ">" in line:
    start_position = line.find("<")
    end_position = line.find(">")
    variable_name = line[start_position + 1:end_position].strip()
    if variable_name in resolved:
      resolved_value = resolved[variable_name]
      return line[0:start_position] + str(resolved_value) + line[end_position + 1:]
  return line

def process_line(line):
  if process_set_variable(line):
    return
  if process_set_macro(line):
    return
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






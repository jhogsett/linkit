#!/usr/bin/python

import os
import app_ui as ui
import terminal_colors as tc
import long_commands as lc
import code

global macros, macro_commands, resolved, unresolved, passes, next_available_macro_number, next_available_sequencer_number
global verbose_mode, starting_macro_number, ending_macro_number, presets, number_of_sequencers, number_of_macros
global led_command, final_macro_numbers, saved_bad_script, includes, last_macro_bytes
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

########################################################################
## API methods
########################################################################

def begin(led_command_, verbose_mode_, quiet_mode, presets_, starting_macro, ending_macro, number_of_sequencers_, bytes_per_macro_, max_string_length_, last_macro_bytes_):
    global verbose_mode, starting_macro_number, ending_macro_number, presets, number_of_sequencers, number_of_macros
    global led_command, bytes_per_macro, max_string_length, next_available_macro_number, last_macro_bytes
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
    tc.begin(quiet_mode)
    ui.begin(verbose_mode, quiet_mode)
    ui.report_verbose("Beginning compilation engine")

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


########################################################################
## global state management
########################################################################

def reset():
    global macros, macro_commands, resolved, unresolved, passes, next_available_macro_number, next_available_sequencer_number
    global final_macro_numbers, includes
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


########################################################################
## compilation main entry point
########################################################################

def compile_script(script):
    global saved_bad_script
    ui.report_verbose("--------------- compilation stating ---------------")

    new_script = resolve_script(script)
    new_lines = consolidate_macros(new_script)
    sort_script(new_lines)

    ui.report_verbose_alt("consolidated script with proxied macro numbers")
    if verbose_mode:
        print_script(new_lines)

    if not compilation_valid(new_lines):
        saved_bad_script = new_lines
        raise ValueError("The script did not compile successfully due to unresolved values.")

    ui.report_verbose("--------------- post compilation processing ---------------")
    new_lines = post_processing(new_lines)
    ui.report_verbose_alt("script after final macro number assignment")
    if verbose_mode:
        print_script(new_lines)

    ui.report_verbose("--------------- post compilation clean up ---------------")
    new_lines = post_clean_up(new_lines)
    ui.report_verbose_alt("script after cleanup:")
    if verbose_mode:
        print_script(new_lines)

    ui.report_verbose("--------------- comnpilation complete ---------------")
    return new_lines


########################################################################
## file inclusion routines
########################################################################

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

## ----------------------------------------------------

def remove_fixed_macro_numbers(line):
    start, end = locate_delimiters(line, "[", "]")
    if start != -1 and end != -1:
        args = extract_args(line, "[", "]")
        if len(args) == 2 and is_number(args[1]):
            # leave only the name
            return "[" + args[0] + "]"
    return line

## ----------------------------------------------------

def rewrite_included_script_line(script_line):
    new_line = remove_fixed_macro_numbers(script_line)
    return new_line

## ----------------------------------------------------

def rewrite_included_script_lines(script_lines):
    new_lines = []
    for line in script_lines:
        line = line.strip()
        new_lines.append(rewrite_included_script_line(line))
    return new_lines


########################################################################
# Main compilation engine
########################################################################

def resolve_script(script_lines):
    if verbose_mode:
        ui.report_verbose("script before pre-processing:")
        print_script(script_lines)
        ui.report_verbose()

    ui.report_verbose("--------------- Pre-processing ---------------")
    new_lines = pre_process_script(script_lines)

    if verbose_mode:
        ui.report_verbose("pre-processed script:")
        print_script(new_lines)
        ui.report_verbose()

    ui.report_verbose("--------------- Initial processing ---------------")
    new_lines = resolution_pass(new_lines)

    ui.report_verbose("script after initial resolution pass:")
    if verbose_mode:
        print_script(new_lines)

    proxy_macro_numbers()

    ui.report_verbose("script after proxy macro number assignment:")
    if verbose_mode:
        print_script(new_lines)

    ui.report_verbose("--------------- Main processing ---------------")
    while True:
        prev_lines = new_lines
        new_lines = resolution_pass(new_lines)
        if new_lines == prev_lines:
            # no more resolving needed/possible
            break
        proxy_macro_numbers()
    return new_lines


########################################################################
## pre-processing routines
########################################################################

def pre_process_script(script_lines):
    new_lines = remove_blank_lines(script_lines)
    new_lines = remove_comments(new_lines)
    new_lines = pre_rewrite(new_lines)
    new_lines = translate_commands(new_lines)
    new_lines = process_directives(new_lines)
    new_lines = capture_templates(new_lines)
    new_lines = process_get_variables(new_lines)
    new_lines = process_set_variables(new_lines)
    new_lines = expand_meta_templates(new_lines)
    new_lines = expand_templates(new_lines)
    return new_lines

## ----------------------------------------------------

def remove_blank_lines(script_lines):
    new_lines = []
    for line in script_lines:
        line = process_blank_line(line)
        if len(line) > 0:
            new_lines.append(line)
    return new_lines

## ----------------------------------------------------

def remove_comments(script_lines):
    new_lines = []
    for line in script_lines:
        line = process_comment(line)
        if len(line) > 0:
            new_lines.append(line)
    return new_lines

## ----------------------------------------------------

# rewrite the script in the new style without the colons
# this simplifies automatic modifying of the script
def pre_rewrite(script_lines):
    new_lines = []
    for line in script_lines:
        segments = line.split(":")
        for segment in segments:
            new_lines.append(segment)
    ui.report_verbose("pre_rewrite script after pre-rewrite:")
    if verbose_mode:
        print_script(new_lines)
    return new_lines

## ----------------------------------------------------

def process_set_variables(script_lines):
    new_lines = []
    for line in script_lines:
        line = process_set_variable(line)
        if len(line) > 0:
            new_lines.append(line)
    return new_lines

def process_get_variables(script_lines):
    new_lines = []
    for line in script_lines:
        line = process_get_variable(line)
        if len(line) > 0:
            new_lines.append(line)
    return new_lines

# this assumes all commands are on individual lines
def translate_commands(script_lines):
    new_lines = []
    for line in script_lines:
        line = line.strip()
        line = lc.translate(line)
        new_lines.append(line)
    ui.report_verbose("translate_commands script after command translation:")
    if verbose_mode:
        print_script(new_lines)
    return new_lines

## ----------------------------------------------------

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

## ----------------------------------------------------

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

## ----------------------------------------------------

def expand_meta_templates(script_lines):
    new_lines = []
    for line in script_lines:
        line = line.strip()
        args = extract_args(line, "(((", ")))")
        if len(args) >= 2:
            # do any variable replacements that can be done
            line = replace_all_variables(line)
            args = extract_args(line, "(((", ")))")
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
                except ValueError:
                    # argument may be a variable reference
                    value = extract_contents(index_arg, "<", ">")
                    if len(value) > 0:
                        if value in resolved:
                            index_max = int(resolved[value])
                        else:
                            raise ValueError("Meta template cannot be expanded due to unresolved variable reference: " + value);
                    else:
                        raise ValueError("Meta template cannot be expanded due to unresolved non-integer argument: " + index_arg);
                except:
                    raise ValueError("Meta template cannot be expanded due to unprocessable argument: " + index_arg);

            # remaining arguments, if any, are the search replacements
            replacements = " ".join(args[2:])
            for index in range(0, index_max):
                new_line = "((" + template_name + " " + str(index) + " " + replacements + "))"
                new_lines.append(new_line)
        else:
            new_lines.append(line)
    return new_lines

## ----------------------------------------------------

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


########################################################################
## pre-processing helpers
########################################################################

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


########################################################################
## main processing routines
########################################################################

def resolution_pass(script_lines):
    global passes
    new_lines = []
    for line in script_lines:
        new_line = process_line(line)
        if new_line != None and new_line != '': #@@@@@:
            new_lines.append(new_line)
    passes += 1
    if verbose_mode:
        ui.report_verbose("--------------------- resolution_pass pass #" + str(passes))
    report_progress()
    new_lines = filter(None, new_lines)
    return new_lines

## ----------------------------------------------------

def process_line(line):
    #ui.report_verbose("--------------------- process_line() ---------------------")
    line = process_blank_line(line)
    line = process_comment(line)
    line = process_evaluate_python(line)
    line = process_set_variable(line)
    line = process_set_macro(line)
    line = process_macro_call(line)
    line = process_get_variable(line)
    line = process_allocate_sequencer(line)
    line = process_place_template(line)
    line = process_configure(line)
    return line

## ----------------------------------------------------

# remove excess whitespace on blank lines
def process_blank_line(line):
    line = line.strip()
    if len(line) == 0:
        return ''

    return line

## ----------------------------------------------------

# strip comment from line
def process_comment(line):
    line = line.strip()
    if len(line) == 0:
        return ''

    if line[0] == "#":
        return ''

    if "#" in line:
        return line.split("#")[0]

    return line

## ----------------------------------------------------

# how to process a line like:
# ``3*3`*2`

def process_evaluate_python(line):
    line = line.strip()
    if len(line) < 1:
        return ''

    # see if line has a python expression
    expression = extract_contents(line, "`", "`", True)
    if len(expression) > 0:

        # the line may have multiple expressions as arguments
        segments = line.split(',')

        new_line = []
        for segment in segments:
            expression = extract_contents(segment, "`", "`", True)

            # clean up the expression, removing excess backticks
            expression = "".join(expression.split('`'))

            if len(expression) > 0:
                # can only process python expression if there are no unresolves values
                #iif not line_has_unresolved_variables(expression):
                #if not line_has_unresolved(expression):
		if not line_has_unresolved_for_python_evaluation(expression):
                    #ui.report_verbose_alt("-evaluating Python: " + expression)
                    result = eval(expression)
#todo-catch error
                    #ui.report_verbose_alt("=evaluated result: " + str(result))
                    #ui.report_verbose_alt("process_evaluate_python replacing python expression '{}' with '{}'".format(expression, result))
                    new_line.append(replace_args(segment, "`", "`", str(result), True))
                else:
                    #ui.report_verbose_alt2("skipping segment with unresolved: " + expression)
                    new_line.append(segment)
            else:
                new_line.append(segment)
        result = ",".join(new_line)
        #ui.report_verbose_alt2("line returned by process_evaluate_python: " + result)
        return result

    # return the unprocessed line
    # ui.report_verbose("process_evaluate_python returning unprocessed line '{}'".format(line))
    return line

## ----------------------------------------------------

def process_set_variable(line):
    line = line.strip()
    if len(line) < 1:
        return ''

    # can only process if there are no unresolved values
    # ? does this leave out variable unresolves?
    if not line_has_unresolved(line):

        # see if line has a variable setting
        args = get_key_args(line, "$")
        if len(args) >= 2:

            variable_name = args[0]

            # instead of taking arg #2, set the variable to the remainder of the line, so it can include spaces
            variable_value = line[len(variable_name) + 1:].strip()

            # can only set if not already set, or a preset that allows overwriting
            if not immutable_resolved_value(variable_name, variable_value):
                #ui.report_verbose("process_set_variable settings {}={}".format(variable_name, variable_value))
                set_resolved(variable_name, variable_value)

            # return a blank line now that this one is consumed
            return ''

    # return the unprocessed line
    # ui.report_verbose("process_set_variable returning unprocessed line '{}'".format(line))
    return line

## ----------------------------------------------------

# process macro setting if present in line
def process_set_macro(line):
    line = line.strip()
    if len(line) == 0:
        return ''

    # can't process the macro setting if there are unresolved values
    if line_has_unresolved_variables(line):
        #ui.report_verbose_alt2("skipping processing macro on line: " + line)
        return line

    macro_name = None
    macro_number = None

    # get arguments inside brackets
    # arg #1 is name of macro
    # arg #2 is the forced macro number
    # no processing occurs if there are no arguments
    args = extract_args(line, "[", "]")

    if len(args) > 0:
        macro_name = args[0]

        if len(args) > 1:
            macro_number = args[1]

        if macro_number == None:
            # there was no forced macro number, so this macro
            # will need an assigned number
            # record it as unresolved variable
            set_unresolved(macro_name)
            ui.report_verbose("process_set_macro new unresolved macro: " + macro_name)

            # convert the line to a simple variable reference
            return "<" + macro_name + ">:set"

        else:
            # there is a forced macro number

            if macro_number == "!":
                # the magic macro number ! means assign the
                # final macro number, which commonly has fewer
                # available bytes
                # used for simple rendering macro
                #ui.report_verbose("- forced final macro: " + macro_name)
                macro_number = ending_macro_number

            macro_number = int(macro_number)
            set_final_macro_number(macro_number, macro_number)
            ui.report_verbose("process_set_macro new forced macro: {} {}".format(macro_name, macro_number))

            set_resolved(macro_name, macro_number)
            set_macro(macro_name, macro_number)

            # setting a specific macro number is done for apps
            # to combine apps, the next available sequence number
            # needs to be reset. 
            # if a macro number is passed, assume it's an app
            # and do the resetting (unless it's the last)
            #needed????????
#            if int(macro_number) != ending_macro_number:
#                reset_next_available_sequence_number()

            # replace with a proxy macro number marker
            # marked by being < 0
            proxy_macro_number = str(macro_number * -1)
            ui.report_verbose_alt("process_set_macron new proxy macro number marker: " + proxy_macro_number)
            return "'" + proxy_macro_number + "':set"

    # return the unprocessed line
    # ui.report_verbose("process_set_macro returning unprocessed line '{}'".format(line))
    return line

## ----------------------------------------------------

# process macro call if in line
def process_macro_call(line):
    macro_name = None
    line = line.strip()
    args = extract_args(line, "(", ")")

    if len(args) > 0:
        macro_name = args[0]

        # can only process if macro name is recognized
        if macro_name in resolved:

            # the second argument is the schedule
            if len(args) > 1:
                # the macro will be scheduled
                macro_schedule = args[1]

                # can only schedule macro if the schedule is resolved
                if not line_has_unresolved(macro_schedule):
                    #ui.report_verbose("new macro schedule {} {}".format(macro_name, macro_schedule))
                    return str(macro_schedule) + "," + str(resolved[macro_name]) + ":sch"
            else:
                # the macro will be run
                #ui.report_verbose("new macro call " + macro_name)
                return str(resolved[macro_name]) + ":run"

    # return the unprocessed line
    # ui.report_verbose("process_macro_call returning unprocessed line '{}'".format(line))
    return line

## ----------------------------------------------------

def process_get_variable(line):
    line = line.strip()
    if len(line) < 1:
        return ''

    # see if line has a variable reference
    args = extract_args(line, "<", ">")
    if len(args) > 0:
        variable_name = args[0]

        if variable_name in resolved:
            # replace the variable reference with the resolved value
            resolved_value = resolved[variable_name]
            #ui.report_verbose("process_get_variable replacing variable reference '{}' with '{}'".format(variable_name, resolved_value))
            return replace_args(line, "<", ">", resolved_value)
        else:
            #ui.report_verbose_alt2("variable not found: " + variable_name)
            pass

    # return the unprocessed line
    # ui.report_verbose("process_get_variable returning unprocessed line '{}'".format(line))
    return line

## ----------------------------------------------------

def process_allocate_sequencer(line):
    global next_available_sequencer_number

    line = line.strip()
    if len(line) < 1:
        return ''

    # see if line has a sequencer allocation
    #ui.report_verbose_alt2("process_allocate_sequencer " + line)
    args = extract_args(line, "{", "}")
    if len(args) > 0:

        sequencer_name = args[0]
        resolved_value = None

        if sequencer_name in resolved:
            # this sequencer has already been allocated
            # reuse this sequencer
            resolved_value = resolved[sequencer_name]

        else:
            # allocate a new sequencer
            if next_available_sequencer_number > (number_of_sequencers - 1):
                raise ValueError("No available sequence numbers available")

            resolved_value = next_available_sequencer_number
            ui.report_verbose("process_allocate_sequencer allocating sequencer: " + str(resolved_value))

            # save new allocated sequencer as resolved value
            set_resolved(sequencer_name, resolved_value)
            next_available_sequencer_number += 1

        #ui.report_verbose("process_allocate_sequencer replacing sequencer allocation {} with {}".format(sequencer_name, resolved_value))
        return replace_args(line, "{", "}", str(resolved_value))

    # return the unprocessed line
    # ui.report_verbose("process_allocate_sequencer returning unprocessed line '{}'".format(line))
    return line

## ----------------------------------------------------

def process_place_template(line):
    line = line.strip()
    if len(line) < 1:
        return ''

    # see if line has a template expqnsion
    args = extract_args(line, "((", "))")
    if len(args) > 0:

        template_name = args[0]

        # can only expand if there are no unresolved values
        if template_name in resolved:
            template_script = resolved[template_name]
            #ui.report_verbose("-placing template: " + template_name)
            ui.report_verbose("process_place_template expanding template " + template_name)
            return replace_args(line, "((", "))", template_script)

    # return the unprocessed line
    # ui.report_verbose("process_place_template returning unprocessed line '{}'".format(line))
    return line    

## ----------------------------------------------------

def process_configure(line):
    line = line.strip()
    if len(line) < 1:
        return ''

    # see if line has a configuration shortcut
    if "=" in line:
        halves = line.split("=")
        if len(halves) == 2:
            halves[0].strip()
            halves[1].strip()
            if len(halves[0]) > 0 and len(halves[1]) > 0:
                return halves[0] + "," + halves[1] + ":cfg"
    return line


########################################################################
## main processing helpers
########################################################################

# join macros that span multiple lines
# into a single line
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


########################################################################
## post processing routines
########################################################################

def post_processing(script_lines):
    script_lines.sort()
    return assign_final_macro_numbers(script_lines)

## ----------------------------------------------------

def do_clean_ups(script_lines, clean_ups):
        new_lines = []
        for line in script_lines:
                for clean_up in clean_ups:
                        line = line.replace(clean_up, clean_ups[clean_up])
                new_lines.append(line)
        return new_lines

## ----------------------------------------------------

def post_clean_up(script_lines):
        clean_ups = {
                # remove unnecessary (zero) arguments
                ",0:" : ":",
                ",0,0:" : ":",
                ":0,0,0:" : ":"
        }
        return do_clean_ups(script_lines, clean_ups)


########################################################################
## macro number assignment routines
########################################################################

# expects a proxy macro number in the form '4'
def is_macro_number_in_use(macro_number):
    for value in macros.values():
        if "'" in str(value):
            value = int(value[1:-1])
        if value == macro_number:
         return True
    return False

## ----------------------------------------------------

def get_next_macro_number():
    global next_available_macro_number
    while next_available_macro_number <= ending_macro_number:
        if not is_macro_number_in_use(next_available_macro_number):
            return next_available_macro_number
        else:
            next_available_macro_number += 1
    if next_available_macro_number > ending_macro_number:
        raise ValueError("No available macro numbers available")

## ----------------------------------------------------

# assign tentative macro numbers so everything else can resolve
# these will be resolved to real macro numbers later
def proxy_macro_numbers():
    #ui.report_verbose("proxy_macro_numbers unresolved:")
    #if verbose_mode:
    #    print_script(unresolved)

    names = unresolved.keys()
    names.sort()
    for name in names:
        if unresolved[name] == None:
            new_macro_number = get_next_macro_number()
            # proxy numbers will be in the form '10' 
            ui.report_verbose("proxy_macro_numbers assigning proxy macro #" + str(new_macro_number) + " for macro: " + name)
            proxy_macro_value = "'" + ("0" + str(new_macro_number))[-2:] + "'"
            resolve_unresolved(name, new_macro_number)
            set_resolved(name, proxy_macro_value)
            set_macro(name, proxy_macro_value)
    remove_resolved()

## ----------------------------------------------------

def assign_final_macro_number(line):
    global saved_bad_script

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
        ui.report_verbose("assign_final_macro_number measuring proxy macro #" + str(proxy_macro_number) + " on device")
        bytes_used = led_command.command_int(test_macro)
        ui.report_verbose_alt("assign_final_macro_number reported size: " + str(bytes_used) + " bytes")
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

        # see if there's room in case this macro needs to overflow
        # raise an error if it runs into another forced macro number
        for x in range(1, macro_slots_required):
            try_macro_number = proxy_macro_number + x

            if try_macro_number in get_final_macro_numbers().values():
                saved_bad_script = [line]
                raise ValueError("No block of macros for fixed macro during final number assignment")

        final_macro_number = proxy_macro_number
    else:
        # compute range of macro numbers to try
        potential_macro_number = starting_macro_number
        last_potential_macro_number = ending_macro_number - (macro_slots_required - 1)

        retry = False

        # go thru each possibly usable macro number
        while potential_macro_number <= last_potential_macro_number:
            #ui.report_verbose("trying macro number: " + str(potential_macro_number))

            # check additional adjacent macro numbers in the case of overflowing macros
            for x in range(0, macro_slots_required):

                try_macro_number = potential_macro_number + x
                if try_macro_number in get_final_macro_numbers().values():
                    #ui.report_verbose("macro #%d already in use" % try_macro_number)
                    retry = True
                    break

            if retry:
                # need to look for another clear block of macro numbers
                retry = False

                potential_macro_number += 1
                #ui.report_verbose("trying next macro #%d" % potential_macro_number)

                if potential_macro_number > ending_macro_number:
                    saved_bad_script = [line]
                    raise ValueError("No available macro numbers available during final number assignment")

                continue
            # open block of macro numbers founds
            break

        final_macro_number = potential_macro_number

    ui.report_verbose_alt("assign_final_macro_number assigning final macro #" + str(final_macro_number) + " for proxy #" + str(proxy_macro_number))
    report_progress()
    set_final_macro_number(proxy_macro_number, final_macro_number)

    # FIX
    if final_macro_number == ending_macro_number and bytes_used > last_macro_bytes:
        raise ValueError("Not enough remaining bytes in the last macro position for Macro %d" % final_macro_number)

    # consume any additional macro numbers to account for byte overage
    consumed_macro_number = final_macro_number
    remaining_bytes = bytes_used - usable_bytes_per_macro

    while remaining_bytes > 0:
        consumed_macro_number += 1
        remaining_bytes -= usable_bytes_per_macro

        # create a unique key to hold the additional consumed macro number value
        if consumed_macro_number in get_final_macro_numbers().values():
            saved_bad_script = [line]
            raise ValueError("Macro {} is needed as a carry-over macro but is already assigned".format(consumed_macro_number))

        ui.report_verbose("assign_final_macro_number allocating macro #" + str(consumed_macro_number) + " to macro #" + str(proxy_macro_number) + " overflow") 
        set_overflow_macro_number(proxy_macro_number, consumed_macro_number)
    
    # return the line with the proxy macro number replaced so it's not processed a second time
    return replace_args(line, "'", "'", str(final_macro_number)) 

## ----------------------------------------------------

def process_finalized_macro_numbers_pass(script_lines):
    new_lines = []

    for line in script_lines:
        args = extract_args(line, "'", "'")
        if len(args) == 1:
            proxy_macro_number = int(args[0])

            if proxy_macro_number in get_final_macro_numbers():
                final_macro_number = get_final_macro_numbers()[proxy_macro_number]
                new_line = replace_args(line, "'", "'", final_macro_number)
                new_lines.append(new_line)

            else:
                new_lines.append(line)
        else:
            new_lines.append(line)

    return new_lines

## ----------------------------------------------------

def process_finalized_macro_numbers(script_lines):
    processed_lines = script_lines

    while True:
        prev_lines = processed_lines

        if verbose_mode:
            ui.report_verbose("process_finalized_macro_numbers pass")
        report_progress()

        processed_lines = process_finalized_macro_numbers_pass(processed_lines)
        if processed_lines == prev_lines:
            # no more resolving is possible
            return processed_lines

## ----------------------------------------------------

# first pass - assign final macro numbers, measure programmed size,
# and allocate any additional macro slots needed for byte overage
def assign_final_macro_numbers_pass_one(script_lines):
    new_lines = []
    for line in script_lines:
        new_lines.append(assign_final_macro_number(line))
    return new_lines

## ----------------------------------------------------

# second pass - replace proxy macro numbers with final numbers
def assign_final_macro_numbers_pass_two(script_lines):
    new_lines = process_finalized_macro_numbers(script_lines)
    return new_lines

## ----------------------------------------------------

def assign_final_macro_numbers(script_lines):
    processed_lines = script_lines
    while True:
        ui.report_verbose("assign_final_macro_numbers pass")
        prev_lines = processed_lines
        processed_lines = assign_final_macro_numbers_pass_one(processed_lines)
        processed_lines = assign_final_macro_numbers_pass_two(processed_lines)
        if processed_lines == prev_lines:
            # no more processing is possible
            return processed_lines


########################################################################
## line processing helpers
########################################################################

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

def line_has_unresolved_for_python_evaluation(line):
    return line_has_unresolved_variables(line) or line_has_python_expression(line) or line_has_template_marker(line) or line_has_sequence_marker(line)

########################################################################
## line maniupation routines
########################################################################

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

## ----------------------------------------------------

def cut_contents(line, start_delimiter, end_delimiter, start, end):
    return line[start + len(start_delimiter):end].strip()

## ----------------------------------------------------

# pass in line and two delimiters, get back contents within
# delimiters specified as one or two characters
def extract_contents(line, start_delimiter, end_delimiter, outer=False):
    line = line.strip()
    if len(line) == 0:
        return ''
    start, end = locate_delimiters(line, start_delimiter, end_delimiter, outer)
    if start != -1 and end != -1:
        return cut_contents(line, start_delimiter, end_delimiter, start, end)
    return ''

## ----------------------------------------------------

# pass in line and two delimiters, get back list of arguments within
# delimiters specified as one or two characters
def extract_args(line, start_delimiter, end_delimiter):
    return extract_contents(line, start_delimiter, end_delimiter).split()

## ----------------------------------------------------

def get_key_contents(line, key):
    line = line.strip()
    if len(line) > 0 and line.startswith(key):
        return line[len(key):].strip()
    return ''

## ----------------------------------------------------

def get_key_args(line, key):
    return get_key_contents(line, key).split()

## ----------------------------------------------------

def replace_args(line, start_delimiter, end_delimiter, replacement, outer=False):
    start, end = locate_delimiters(line, start_delimiter, end_delimiter, outer)
    if start != -1 and end != -1:
        return line[0:start] + str(replacement) + line[end + 1:]
    return line

def replace_all_variables(line):
    while True:
        prev_line = line
        line = process_get_variable(line)
        if line != prev_line:
            continue
        break
    return line


########################################################################
## management of resolved and unresolved values
########################################################################

def set_resolved(name, value):
    #ui.report_verbose("set_resolved name: {} ({}) value: {} ({})".format(name, type(name), value, type(value)))
    resolved[name] = value

def set_unresolved(name, value=None):
    #ui.report_verbose("set_unresolved name: {} ({}) value: {} ({})".format(name, type(name), value, type(value)))
    unresolved[name] = value

# sets a value for an unresolved value, resolving it
# proxy for set_unresolved()
def resolve_unresolved(name, value=None):
    #ui.report_verbose("resolve_unresolved name: {} ({}) value: {} ({})".format(name, type(name), value, type(value)))
    unresolved[name] = value

# removed an resolved values that now have values
# and are therefore resolved
def remove_resolved():
    global unresolved
    new_dict = {}
    for name in unresolved:
        if unresolved[name] == None:
            new_dict[name] = None
        else:
            #ui.report_verbose("removing resolved unresolved name: {} value: {}".format(name, unresolved[name]))
            pass
    unresolved = new_dict

# return true if resolved value is a mutable preset and can be changed
# raise ValueError if resolved value is immutable value being changed
def immutable_resolved_value(variable_name, variable_value):
    if variable_name in resolved:
        if resolved[variable_name] != variable_value:
            if variable_name in presets:
                return True
            else:
                raise ValueError("The immutable resolved value '%s' is being changed to '%s'" % (variable_name, str(variable_value)))
    return False

# handles key=value presents passed on command line
# pre-assigns those values to variables
def resolve_presets(presets):
    for key in presets.keys():
        #ui.report_verbose("setting preset resolved value " + tc.yellow(key + "=" + str(presets[key])))
        set_resolved(key, presets[key])


########################################################################
## general table management
########################################################################

def set_macro(name, value):
    global macros
    macros[name] = value

def set_final_macro_number(proxy_macro_number, final_macro_number):
    if not type(proxy_macro_number) is int:
        raise TypeError("proxy_macro_number must be a 'int'")
    if not type(final_macro_number) is int:
        raise TypeError("final_macro_number must be a 'int'")
    #ui.report_verbose("set_final_macro_number proxy_macro_number: {} final_macro_number: {}".format(proxy_macro_number, final_macro_number))
    final_macro_numbers[proxy_macro_number] = final_macro_number

def set_overflow_macro_number(proxy_macro_number, consumed_macro_number):
    if not type(proxy_macro_number) is int:
        raise TypeError("proxy_macro_number must be a 'int'")
    if not type(consumed_macro_number) is int:
        raise TypeError("consumed_macro_number must be a 'int'")
    #ui.report_verbose("set_overflow_macro_number proxy_macro_number: {} consumed_macro_number: {}".format(proxy_macro_number, consumed_macro_number))
    key = str(proxy_macro_number) + "-" + str(consumed_macro_number)
    final_macro_numbers[key] = consumed_macro_number

def get_final_macro_numbers():
    return final_macro_numbers

def get_saved_bad_script():
    return saved_bad_script

def get_resolved():
    return resolved

def get_unresolved():
    return unresolved

def get_presets():
    return presets

def get_macros():
    return macros

def get_includes():
    return includes


########################################################################
## general helpers
########################################################################

def is_number(str):
    try:
        int(str)
        return True
    except ValueError:
        return False

def print_script(script):
    for line in script:
        ui.report_error(line)
    print

def sort_script(script_lines):
    script_lines.sort(reverse=False)

def report_progress():
        if not verbose_mode:
                ui.write(tc.green("."))


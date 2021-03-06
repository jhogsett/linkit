#!/usr/bin/python

import os
import app_ui as ui
#import terminal_colors as tc
import long_commands as lc
import code
import utils
import re
import macro_language as ml

global macros, macro_commands, resolved, unresolved, passes, next_available_macro_number, next_available_sequencer_number
global verbose_mode, starting_macro_number, ending_macro_number, presets, number_of_sequencers, number_of_macros
global led_command, final_macro_numbers, saved_bad_script, includes, last_macro_bytes, allow_mutability, translated, translation
global pre_processed_script

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
allow_mutability = None
translated = []
translation = {}
pre_processed_script = None


########################################################################
## API methods
########################################################################

def begin(led_command_, verbose_mode_, quiet_mode, presets_, starting_macro, ending_macro, number_of_sequencers_, bytes_per_macro_, max_string_length_, last_macro_bytes_, allow_mutability_=False):
    global verbose_mode, starting_macro_number, ending_macro_number, presets, number_of_sequencers, number_of_macros
    global led_command, bytes_per_macro, max_string_length, next_available_macro_number, last_macro_bytes, allow_mutability

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
    allow_mutability = allow_mutability_
    presets = presets_
    resolve_presets(presets)
    add_default_resolved()

    utils.begin(False, __file__)
    #tc.begin(quiet_mode)
    ui.begin(verbose_mode, quiet_mode)
    ui.report_verbose("Beginning compilation engine")

def compile_file(filename):
    script = load_file(filename)
    return compile_script(script)

def compilation_valid(script):
    for line in script:
        if line_has_unresolved(line):
            ui.report_verbose_alt2("Unresolved value in line:\n" + line);
            return False
    return True

def remaining_sequencers():
    return number_of_sequencers - next_available_sequencer_number

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

def get_translated():
    return translated

def get_translation():
    return translation

def get_preprocessed():
    return pre_processed_script


########################################################################
## global state management
########################################################################

def reset(presets_={}):
    global macros, macro_commands, resolved, unresolved, passes, next_available_macro_number, next_available_sequencer_number, allow_mutability
    global final_macro_numbers, includes, presets, translated, translation

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

    if len(presets_):
      presets = presets_

    resolve_presets(presets)
    add_default_resolved()
    allow_mutability = False
    translated = []
    translation = {}

def reset_next_available_sequence_number():
    next_available_sequencer_number = 0


########################################################################
## compilation main entry point
########################################################################

def compile_script(script):
    global saved_bad_script

    ui.report_verbose()
    ui.report_verbose_header("Compilation Starting", "=")

    new_script = resolve_script(script)
    new_lines = consolidate_macros(new_script)
    ui.report_verbose_script(new_lines, "consolidated script with proxied macro numbers")

    utils.sort_script(new_lines)
    ui.report_verbose_script(new_lines, "script after sorting")

    if not compilation_valid(new_lines):
        saved_bad_script = new_lines
        raise ValueError("The script did not compile successfully due to unresolved values.")

    ui.report_verbose_header("Post Compilation Processing")

    new_lines = post_processing(new_lines)
    ui.report_verbose_script(new_lines, "script after final macro number assignment")

    ui.report_verbose_header("Post Compilation Cleanup")

    new_lines = post_clean_up(new_lines)
    ui.report_verbose_script(new_lines, "script after post clean up")

    ui.report_verbose_header("Compilation Complete", '=')
    return new_lines


########################################################################
## file inclusion routines
########################################################################

def load_file(filename, default_ext=".mac"):
    file_lines = []
    filename = utils.locate_file(filename, default_ext)
    file_path = utils.get_path(filename)
    file = open(filename, "r")
    for line in file:
        line = line.strip()
        if len(line) == 0:
            continue
        if line.startswith(ml.include_marker):
            args = line.split()
            include_filename = args[1]
            if len(include_filename) > 0 and include_filename not in includes.keys():
                full_filename = os.path.join(file_path, include_filename)
                include_lines = load_file(full_filename)
                module_name = os.path.basename(include_filename)
                include_lines, no_prefix = no_prefix_directive_check(include_lines)
                include_lines = remove_macro_numbers(include_lines)
                if not no_prefix:
                    include_lines = add_inclusion_marker(include_lines)
                    include_lines = prefix_module_on_macros(module_name, include_lines)
                    include_lines = prefix_module_on_variables(module_name, include_lines)
                    include_lines = prefix_module_on_templates(module_name, include_lines)
                    include_lines = prefix_module_on_template_macros(module_name, include_lines)
                file_lines = file_lines + include_lines
                includes[include_filename] = full_filename
                continue
        file_lines.append(line)
    return file_lines

## ----------------------------------------------------

def no_prefix_directive_check(script_lines):
    new_lines = []
    result = False
    for line in script_lines:
        line = line.strip()
        if line.startswith(ml.no_prefix_marker):
            result = True
        else:
            new_lines.append(line)
    return new_lines, result

def remove_fixed_macro_numbers(line):
    if not line_has_unresolved_for_include_rewrite(line):
        start, end = utils.locate_delimiters(line, ml.set_macro_delimiters)
        if start != -1 and end != -1:
            args = utils.extract_args(line, ml.set_macro_delimiters)
            if len(args) == 2 and utils.is_number(args[1]):
                # leave only the name
                return ml.set_macro_start + args[0] + ml.set_macro_end
    return line

def delimiters_in_line(line, delimiters):
    for delimiter in delimiters:
        if delimiter in line:
            return delimiter
    return None

## ----------------------------------------------------

# add prefix to the the beginning of the name argument for dual-delimiter arguments
def prefix_module(line, delimiters, non_delimiters, translated, translation, prefix):
    starter_delimiters = delimiters.keys()
    ender_delimiters = delimiters.values()
    starter_non_delimiters = non_delimiters.keys()

    delimiter = delimiters_in_line(line, starter_delimiters)
    if delimiter == None:
        return line

    if len(starter_non_delimiters) > 0 and delimiters_in_line(line, starter_non_delimiters) != None:
        return line

    starter_delimiter = delimiter
    ender_delimiter = delimiters[starter_delimiter]
    start, end = utils.locate_delimiters(line, {starter_delimiter : ender_delimiter})
    if start == -1 or end == -1:
        return line

    args = utils.extract_args(line, {starter_delimiter : ender_delimiter})
    if len(args) < 1:
        return line

    old_name = args[0]
    # skip already-translated macros
    if old_name in translated:
        return line

    new_name = prefix + "-" + old_name
    translation[old_name] = new_name
    translated.append(new_name)
    args[0] = new_name
    line = starter_delimiter + " ".join(args) + ender_delimiter
    return line

# add prefix to the the beginning of the name argument for single-delimiter arguments
def prefix_module_single(line, delimiters, non_delimiters, translated, translation, prefix_name):
    delimiter = delimiters_in_line(line, delimiters)
    if delimiter == None:
        return line

    if len(non_delimiters) > 0 and delimiters_in_line(line, non_delimiters) != None:
        return line

    args = utils.get_key_args(line, delimiter)
    if len(args) < 1:
        return line

    old_name = args[0]
    # skip already-translated macros
    if old_name in translated:
        return line

    new_name = prefix_name + "-" + old_name
    translation[old_name] = new_name
    translated.append(new_name)
    args[0] = new_name
    line = delimiter + " ".join(args)
    return line

def apply_prefixing(line, delimiters, non_delimiters, translation):
    starter_delimiters = delimiters.keys()
    ender_delimiters = delimiters.values()
    starter_non_delimiters = non_delimiters.keys()

    delimiter = delimiters_in_line(line, starter_delimiters)
    if delimiter == None:
        return line

    if len(starter_non_delimiters) > 0 and delimiters_in_line(line, starter_non_delimiters) != None:
        return line

    starter_delimiter = delimiter
    ender_delimiter = delimiters[starter_delimiter]
    start, end = utils.locate_delimiters(line, {starter_delimiter : ender_delimiter})
    if start == -1 or end == -1:
        return line

    args = utils.extract_args(line, {starter_delimiter : ender_delimiter})
    if len(args) < 1:
        return line

    old_name = args[0]
    if old_name not in translation:
        return line

    new_name = translation[old_name]
    args[0] = new_name
    line = starter_delimiter + " ".join(args) + ender_delimiter
    return line

def apply_prefixing_multiple(line, delimiters, non_delimiters, translation):
    starter_delimiters = delimiters.keys()
    ender_delimiters = delimiters.values()
    starter_non_delimiters = non_delimiters.keys()

    delimiter = delimiters_in_line(line, starter_delimiters)
    if delimiter == None:
        return line

    if len(starter_non_delimiters) > 0 and delimiters_in_line(line, starter_non_delimiters) != None:
        return line

    starter_delimiter = delimiter
    ender_delimiter = delimiters[starter_delimiter]
    start, end = utils.locate_delimiters(line, {starter_delimiter : ender_delimiter})
    if start == -1 or end == -1:
        return line

    split_delimiters = { starter_delimiter : ender_delimiter }
    parts = utils.smart_split(line, split_delimiters, True)
    new_parts = []
    for part in parts:
        if part.startswith(starter_delimiter) and part.endswith(ender_delimiter):
            old_name = part.strip()[1:-1]
            if old_name in translation:
                new_name = translation[old_name]
                part = starter_delimiter + new_name + ender_delimiter
        new_parts.append(part)
    line = " ".join(new_parts)
    return line

## ----------------------------------------------------

def inside_block(line, delimiters, in_block):
    starter_delimiter = delimiters.keys()[0]
    ender_delimiter = delimiters[starter_delimiter]
    line = line.strip()
    if in_block:
        if line.startswith(ender_delimiter):
            in_block = False
        return in_block
    if line.startswith(starter_delimiter):
        in_block = True
    return in_block

## ----------------------------------------------------

def rename_macro_headers(script_lines, translation, module_name, within_block):
    new_lines = []
    in_block = False
    for line in script_lines:
        in_block = inside_block(line, ml.set_template_delimiters, in_block)
        if in_block == within_block:
            line = prefix_module(line, ml.set_macro_delimiters, ml.set_macro_non_delmiters, translated, translation, module_name)
        new_lines.append(line)
    return new_lines

def rename_macro_runs(script_lines, translation, module_name, within_block):
    new_lines = []
    in_block = False
    for line in script_lines:
        in_block = inside_block(line, ml.set_template_delimiters, in_block)
        if in_block == within_block:
            line = apply_prefixing(line, ml.run_macro_delimiters, ml.run_macro_non_delimiters, translation)
        new_lines.append(line)
    return new_lines

def rename_macro_variable_references(script_lines, translation, module_name, within_block):
    new_lines = []
    in_block = False
    for line in script_lines:
        in_block = inside_block(line, ml.set_template_delimiters, in_block)
        if (not within_block) or in_block:
            line = apply_prefixing_multiple(line, ml.get_variable_delimiters, ml.get_variable_non_delimiters, translation)
        new_lines.append(line)
    return new_lines

## ----------------------------------------------------

def rename_variable_settings(script_lines, translation, module_name):
    new_lines = []
    for line in script_lines:
        line = line.strip()
        line = prefix_module_single(line, [ml.set_variable_marker], ml.empty_delimiter, translated, translation, module_name)
        new_lines.append(line)
    return new_lines

def rename_variable_references(script_lines, translation, module_name):
    new_lines = []
    conditional = False
    for line in script_lines:
        if ml.conditional_start in line:
            conditional = True
            line = line[len(ml.conditional_start) + 1:]
        line = apply_prefixing_multiple(line, ml.get_variable_delimiters, ml.get_variable_non_delimiters, translation)
        if conditional:
            line = ml.conditional_start + " " + line
            conditional = False

        new_lines.append(line)
    return new_lines

## ----------------------------------------------------

def rename_template_headers(script_lines, translation, module_name):
    new_lines = []
    for line in script_lines:
        line = line.strip()
        line = prefix_module_single(line, [ml.set_template_start], [ml.set_template_non_start], translated, translation, module_name)
        new_lines.append(line)
    return new_lines

def rename_template_expansions(script_lines, translation, module_name, within_block):
    new_lines = []
    in_block = False
    for line in script_lines:
        in_block = inside_block(line, ml.set_template_delimiters, in_block)
        if (not within_block) or in_block:
            line = apply_prefixing(line, ml.expand_template_delimiters, ml.expand_template_non_delimiters, translation)
        new_lines.append(line)
    return new_lines

def rename_meta_templates_and_multi_macros(script_lines, translation, module_name, within_block):
    new_lines = []
    in_block = False
    for line in script_lines:
        in_block = inside_block(line, ml.set_template_delimiters, in_block)
        if (not within_block) or in_block:
            line = apply_prefixing(line, ml.meta_and_multi_delimiters, ml.empty_delimiters, translation)
        new_lines.append(line)
    return new_lines

## ----------------------------------------------------

def prefix_module_on_macros(module_name, script_lines):
    new_lines = rename_macro_headers(script_lines, translation, module_name, False)
    new_lines = rename_macro_runs(new_lines, translation, module_name, False) 
    new_lines = rename_macro_variable_references(new_lines, translation, module_name, False)
    return new_lines

def prefix_module_on_variables(module_name, script_lines):
    new_lines = rename_variable_settings(script_lines, translation, module_name)
    new_lines = rename_variable_references(new_lines, translation, module_name)
    return new_lines

def prefix_module_on_templates(module_name, script_lines):
    new_lines = rename_template_headers(script_lines, translation, module_name)
    new_lines = rename_template_expansions(new_lines, translation, module_name, False)
    new_lines = rename_meta_templates_and_multi_macros(new_lines, translation, module_name, False)
    return new_lines

def prefix_module_on_template_macros(module_name, script_lines):
    new_lines = rename_macro_headers(script_lines, translation, module_name, True)
    new_lines = rename_macro_runs(new_lines, translation, module_name, True)
    new_lines = rename_template_expansions(new_lines, translation, module_name, True)
    new_lines = rename_meta_templates_and_multi_macros(new_lines, translation, module_name, True)
    new_lines = rename_macro_variable_references(new_lines, translation, module_name, True)
    return new_lines

## ----------------------------------------------------

def remove_macro_numbers(script_lines):
    new_lines = []
    for line in script_lines:
        line = line.strip()
        new_lines.append(remove_fixed_macro_numbers(line))
    return new_lines

def add_inclusion_marker(script_lines):
    marker = "$INCLUDED True"
    new_lines = script_lines
    new_lines.append(marker)
    return new_lines


########################################################################
# Main compilation engine
########################################################################

def produce_preprocesssed_script(lines):
    new_lines = []
    for resolve in resolved:
        value = resolved[resolve]
        if type(value) is type(" "):
            if resolve.startswith(ml.directive_marker):
                #include_marker = ml.include_marker
                if not resolve.startswith(ml.include_marker):
                    new_lines.append(resolve + " " + str(resolved[resolve]))
            else:
                new_lines.append(ml.set_variable_marker + resolve + " " + str(resolved[resolve]))
    return new_lines + lines

def resolve_script(script_lines):
    global pre_processed_script

    ui.report_verbose_header("Preprocessing")

    new_lines = pre_process_script(script_lines)
    pre_processed_script = produce_preprocesssed_script(new_lines)
    ui.report_verbose_script(new_lines, "pre-processed script")

    ui.report_verbose_header("Initial Processing")

    new_lines = resolution_pass(new_lines)
    ui.report_verbose_script(new_lines, "script after resolution pass")

    proxy_macro_numbers()
    ui.report_verbose_script(new_lines, "script after proxy macro number assignment")

    ui.report_verbose_header("Main Processing")

    while True:
        prev_lines = new_lines
        new_lines = resolution_pass(new_lines)
        ui.report_verbose_script(new_lines, "script after resolution pass")
        if new_lines == prev_lines:
            # no more resolving needed/possible
            break
        proxy_macro_numbers()
    # remove excess spaces
    new_lines = do_clean_ups(new_lines, {" " : ""})
    return new_lines


########################################################################
## pre-processing routines
########################################################################

def pre_process_script(script_lines):
    ui.report_verbose_script(script_lines, "script before pre-processing")

    new_lines = script_lines
    new_lines = utils.strip_whitespace(new_lines)
    new_lines = utils.strip_comments(new_lines)
    new_lines = pre_rewrite(new_lines)
    ui.report_verbose_script(new_lines, "script after blank line, comment and colon removal")

    new_lines = translate_commands(new_lines)
    ui.report_verbose_script(new_lines, "script after command translation")

    new_lines = process_directives(new_lines)
    ui.report_verbose_script(new_lines, "script after processing directives")
    ingest_directives()

    new_lines = process_set_variables(new_lines)
    ui.report_verbose_script(new_lines, "script after capturing variables")

    new_lines = process_conditionals(new_lines)
    ui.report_verbose_script(new_lines, "script after processing conditionals")

    new_lines = process_blocks(new_lines)
    ui.report_verbose_script(new_lines, "script after processing blocks")

    new_lines = process_get_variables(new_lines)
    ui.report_verbose_script(new_lines, "script after replacing variables")

    new_lines = capture_expand_loop(new_lines)
    ui.report_verbose_script(new_lines, "script after capture-expand loop")

    new_lines = expand_meta_loop(new_lines)
    ui.report_verbose_script(new_lines, "script after expand-meta loop")
    return new_lines

def capture_expand_loop(script_lines):
    orig_lines = script_lines
    new_lines = script_lines
    passnum = 1
    while(True):
        ui.report_verbose_alt("capture_expand_loop pass #" + str(passnum))
        passnum += 1
        orig_lines = new_lines

        new_lines = capture_templates(new_lines)
        ui.report_verbose_script(new_lines, "script after capturing templates")

        new_lines = expand_templates(new_lines)
        ui.report_verbose_script(new_lines, "script after expanding templates")

        new_lines = expand_multi_macros(new_lines)
        ui.report_verbose_script(new_lines, "script after expanding multi-macros")

        if new_lines == orig_lines:
            break
    return new_lines

def expand_meta_loop(script_lines):
    new_lines = script_lines
    passnum = 1
    while(True):
        ui.report_verbose_alt("expand_meta_loop pass #" + str(passnum))
        passnum += 1
        orig_lines = new_lines

        new_lines = expand_meta_templates(new_lines)
        ui.report_verbose_script(new_lines, "script after expanding meta templates")

        new_lines = expand_templates(new_lines)
        ui.report_verbose_script(new_lines, "script after expanding templates")

        if new_lines == orig_lines:
            break
    return new_lines

## ----------------------------------------------------

def remove_comments(script_lines):
    return utils.strip_comments(script_lines)

## ----------------------------------------------------

# rewrite the script in the new style without the colons
# this simplifies automatic modifying of the script
def pre_rewrite(script_lines):
    new_lines = []
    for line in script_lines:
        segments = line.split(":")
        for segment in segments:
            new_lines.append(segment)
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

## ----------------------------------------------------

# this assumes all commands are on individual lines
def translate_commands(script_lines):
    new_lines = []
    for line in script_lines:
        line = line.strip()
        line = lc.translate(line)
        new_lines.append(line)
    return new_lines

## ----------------------------------------------------

def process_directives(script_lines):
    new_lines = []
    for line in script_lines:
        args = utils.get_key_args(line, ml.directive_marker)
        if len(args) > 0:
            directive_name = ml.directive_marker + args[0]
            if len(args) == 1:
                directive_value = True
            elif len(args) == 2:
                directive_value = args[1]
            elif len(args) == 3:
                directive_value = { args[2] : args[3] }
            else:
                directive_value = args[1:].split()
            set_resolved(directive_name, directive_value)
        else:
            new_lines.append(line)
    return new_lines

def ingest_directives():
    global allow_mutability
    if ml.allow_mutability_marker in resolved:
        allow_mutability = True

## ----------------------------------------------------

def process_conditionals(script_lines):
    new_lines = []
    builder = []
    capture_mode = False
    bypass_mode = False
    expression = None
    for line in script_lines:
        line = line.strip()
        line = replace_all_variables(line)

        if capture_mode:
            if line.startswith(ml.conditional_end):
                capture_mode = False
                if not bypass_mode:
                    new_lines += builder
                bypass_mode = False
                builder = []
            else:
                builder.append(line)
        else:
            
            if ml.conditional_start in line:
                capture_mode = True
                expression = utils.get_key_contents(line, ml.conditional_start)
                expression = replace_all_variables(expression)
                python_expression = utils.extract_contents(expression, ml.python_delimiters, True)

                if len(python_expression) > 0:
                    # evaluate python expression as provided
                    expression = python_expression
                else:
                    # turn into a python expression expected to evaluate to True
                    expression = "bool( " + expression + " ) == True"

                ui.report_verbose("evaluating conditional expression: " + expression)

                result = eval(expression)
                ui.report_verbose_alt("conditional expression result: " + str(result))

                if result != True:
                    bypass_mode = True
            else:
                new_lines.append(line)
    return new_lines

## ----------------------------------------------------

def process_blocks(script_lines):
    new_lines = []
    for line in script_lines:
        line = line.strip()

        if ml.refresh_block_start in line:
            position = line.find(ml.refresh_block_start)
            new_line = line[:position]
            if len(new_line) > 0:
                new_lines.append(line[:position])

        elif ml.reset_block_start in line:
            position = line.find(ml.reset_block_start)
            new_line = line[:position]
            if len(new_line) > 0:
                new_lines.append(line[:position])

        elif ml.refresh_block_end in line:
            new_lines.append("flu")

        elif ml.reset_block_end in line:
            new_lines.append("rst")

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
            if line.startswith(ml.set_template_end) and not line.startswith(ml.set_template_non_end):
                capture_mode = False
                set_resolved(template_name, template_builder)
                template_builder = []
            else:
                template_builder.append(line)
        else:
            if ml.set_template_start in line and ml.set_template_non_start not in line:
                args = utils.get_key_args(line, ml.set_template_start)
                template_name = args[0]
                check_for_argument_collision(args[1:])
                combined_args = " ".join(args[1:])

                # store the search strings that will be replaced with passed arguments later
                template_builder.append(combined_args)
                capture_mode = True
            else:
                new_lines.append(line)
    return new_lines

def check_for_argument_collision(arguments):
    num_args = len(arguments)
    for a in range(0, num_args):
        for b in range(0, num_args):
            arg_a = arguments[a]
            arg_b = arguments[b]
            if a != b:
                if arg_a == arg_b:
                    raise ValueError("Template has duplicate argument '" + arg_a + "'")

                if arg_a in arg_b or arg_b in arg_a:
                    raise ValueError("Template has interfering arguments '" + arg_a + "' and '" + arg_b + "'")

## ----------------------------------------------------

def expand_multi_macros(script_lines):
    new_lines = []
    add_lines = []
    for line in script_lines:
        line = line.strip()

        # do any variable replacements that can be done
        line = replace_all_variables(line)

        args = utils.extract_args(line, ml.multi_macro_delimiters, ml.python_delimiters)
        if len(args) >= 2:
            macro_name = args[0]
            # remove instance segment from name 

            index = utils.reverse_find(macro_name, "-")
            if index != -1:
                macro_name = macro_name[:index]

            num_instance_arg = args[1]
            num_instance_max = None
            start, end = utils.locate_delimiters(num_instance_arg, ml.python_delimiters)
            if start != -1 and end != -1:
                expression = utils.extract_contents(num_instance_arg, ml.python_delimiters)
                try:
                    num_instance_max = eval(expression)

                except StandardError:
                    raise ValueError("Multi macro cannot be expanded due to unprocessable argument: '" + num_instance_arg + "'");
            else:
                try:
                    num_instance_max = int(num_instance_arg)

                except ValueError:
                    raise ValueError("Multi macro cannot be expanded due to unresolved non-integer argument: " + num_instance_arg);
            # remaining argument, if any, is the optional schedule replacement
            schedule = " ".join(args[2:])
            multi_macro_name = macro_name + "-" + utils.id_generator()
            template_name = multi_macro_name + "-template"

            # replace the multi macro expression with the call to the new macro
            new_lines.append(ml.run_macro_start + multi_macro_name + ml.run_macro_end)

            # create the lines that will be added after this set of lines is processed:
            # add the multi macro
            add_lines.append(ml.set_macro_start + multi_macro_name + ml.set_macro_end)

            # add the meta-template
            add_lines.append("  " + ml.meta_template_start + template_name + " " + str(num_instance_max) + " " + schedule + ml.meta_template_end)

            # add the template
            add_lines.append(ml.set_template_start + template_name + " INSTANCE SCHEDULE")
            add_lines.append(ml.run_macro_start + macro_name + "-INSTANCE SCHEDULE" + ml.run_macro_end)
            add_lines.append(ml.set_template_end)
        else:
            new_lines.append(line)
    return new_lines + add_lines

## ----------------------------------------------------

def expand_meta_templates(script_lines):
    new_lines = []
    for line in script_lines:
        line = line.strip()

        # do any variable replacements that can be done
        line = replace_all_variables(line) 

        args = utils.extract_args(line, ml.meta_template_delimiters, ml.meta_template_grouping)
        if len(args) >= 2:
            template_name = args[0]
            index_arg = args[1]
            index_max = None

            start, end = utils.locate_delimiters(index_arg, ml.python_delimiters)
            if start != -1 and end != -1:
                expression = utils.extract_contents(index_arg, ml.python_delimiters)
                try:
                    index_max = eval(expression)
                except StandardError:
                    raise ValueError("Meta template cannot be expanded due to unprocessable argument: '" + index_arg + "'");                  
            else:
                try:
                    index_max = int(index_arg)
                except ValueError:
                    raise ValueError("Meta template cannot be expanded due to unresolved non-integer argument: " + index_arg);
            replacements = {}
            replacement_args = args[2:]
            num_replacements = len(replacement_args)

            for i in range(0, num_replacements):
                replacement = replacement_args[i]
                if replacement.startswith(ml.set_macro_start) and replacement.endswith(ml.set_macro_end):
                    replacement = replacement[1:-1]
                    parts = replacement.split(",")
                    num_parts = len(parts)

                    if num_parts < index_max:
                        raise ValueError("Meta template cannot be expanded due to fewer than " + str(index_max) + " replacements in list: [" + replacement + "]")

                    parts_list = []
                    for j in range(0, num_parts):
                        part = parts[j].strip()
                        parts_list.append(part)
                    replacements[i] = parts_list
                else:
                    replacements[i] = replacement

            for index in range(0, index_max):
                replacement_round = []

                for j in range(0, len(replacements.keys())):
                    replacement = replacements[j]
                    if isinstance(replacement, list):
                        parts_list = replacement
                        replacement_round.append(parts_list[index])
                    else:
                        replacement_round.append(replacement)
                replacement_str = " ".join(replacement_round)

                new_line = ml.expand_template_start + template_name + " " + str(index) + " " + replacement_str + ml.expand_template_end
                ui.report_verbose_alt("expanded meta template: " + new_line)
                new_lines.append(new_line)
        else:
            new_lines.append(line)
    return new_lines

## ----------------------------------------------------

def expand_templates(script_lines):
    new_lines = []
    for line in script_lines:
        line = line.strip()

        if ml.expand_template_start in line and ml.expand_template_non_start not in line:
            args = utils.extract_args(line, ml.expand_template_delimiters, ml.expand_template_grouping)
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
                if index < len(replacements):
                    replacement = replacements[index]
                else:
                    ui.report_verbose_alt("template replacement not provided for: " + key)
                    replacement = ''
                line = line.replace(key, replacement)
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
        if new_line != None and new_line != '':
            new_lines.append(new_line)

    passes += 1
    ui.report_verbose_header("resolution_pass pass #" + str(passes))
    ui.report_progress()

    new_lines = filter(None, new_lines)
    return new_lines

## ----------------------------------------------------

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
    if line[0] == ml.comment_marker:
        return ''
    if ml.comment_marker in line:
        return line.split(ml.comment_marker)[0]
    return line

## ----------------------------------------------------

def process_evaluate_python(line):
    line = line.strip()
    if len(line) < 1:
        return ''

    # see if line has a python expression
    expression = utils.extract_contents(line, ml.python_delimiters, True)
    if len(expression) > 0:
        # the line may have multiple expressions as arguments
        segments = line.split(',')
        new_line = []

        for segment in segments:
            expression = utils.extract_contents(segment, ml.python_delimiters, True)
            # clean up the expression, removing excess backticks
            expression = "".join(expression.split(ml.python_start))

            if len(expression) > 0:
                # can only process python expression if there are no unresolves values
		if not line_has_unresolved_for_python_evaluation(expression):
                    #ui.report_verbose_alt2("-evaluating Python: " + expression)
                    try:
                        result = eval(expression)

                    except (StandardError) as err:
                        error_message = "Python expression '" + expression + "' raised error " + type(err).__name__ + ": " + str(err)
                        #ui.report_error(error_message)
                        raise ValueError(error_message)

                    #ui.report_verbose_alt2("=evaluated result: " + str(result))
                    #ui.report_verbose_alt2("process_evaluate_python replacing python expression '{}' with '{}'".format(expression, result))
                    new_line.append(utils.replace_args(segment, ml.python_delimiters, str(result), True))
                else:
                    #ui.report_verbose_alt2("skipping segment with unresolved: " + expression)
                    new_line.append(segment)
            else:
                # guard against empty expression
                if segment != ml.empty_python:
                    new_line.append(segment)
        result = ",".join(new_line)
        #ui.report_verbose_alt2("line returned by process_evaluate_python: " + result)
        return result

    # return the unprocessed line
    #ui.report_verbose_alt2("process_evaluate_python returning unprocessed line '{}'".format(line))
    return line

## ----------------------------------------------------

def process_set_variable(line):
    line = line.strip()
    if len(line) > 0:
        # see if line has a variable setting
        args = utils.get_key_args(line, ml.set_variable_marker)
        if len(args) > 0:
            variable_name = args[0]
            if len(args) >= 2:
                if variable_name in presets:
                    # override this variable with the preset
                    return ''

                # instead of taking arg #2, set the variable to the remainder of the line, so it can include spaces
                variable_value = line[len(variable_name) + 1:].strip()

                # can only set if not already set, or a preset that allows overwriting
                if not immutable_resolved_value(variable_name, variable_value):
                    #ui.report_verbose("process_set_variable settings {}={}".format(variable_name, variable_value))
                    set_resolved(variable_name, variable_value)

                # return a blank line now that this one is consumed
                return ''
            else:
                raise ValueError("Variable '" + variable_name + "' was given no value.")
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
    args = utils.extract_args(line, ml.set_macro_delimiters)
    if len(args) > 0:
        macro_name = args[0]
        if is_known_unresolved(macro_name):
            raise ValueError("Macro '" + macro_name + "' cannot be reassigned.")

        if is_known_resolved(macro_name):
            raise ValueError("Macro name '" + macro_name + "' is already in use.")

        if len(args) > 1:
            macro_number = args[1]

        if macro_number == None:
            # there was no forced macro number, so this macro
            # will need an assigned number
            # record it as unresolved variable
            set_unresolved(macro_name)
            ui.report_verbose("process_set_macro new unresolved macro: " + macro_name)

            # convert the line to a simple variable reference
            return ml.get_variable_start + macro_name + ml.get_variable_end + ":set"
        else:
            # there is a forced macro number
            if macro_number == ml.final_macro_marker:
                # the magic macro number ! means assign the
                # final macro number, which commonly has fewer
                # available bytes
                # used for simple rendering macro
                #ui.report_verbose("- forced final macro: " + macro_name)
                macro_number = ending_macro_number
            macro_number = int(macro_number)

            if macro_number_in_use(macro_number):
                raise ValueError("Fixed macro number " + str(macro_number) + " for macro '" + macro_name + "' is already in use.")

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
            ui.report_verbose_alt("process_set_macro new proxy macro number marker: " + proxy_macro_number)

            return ml.proxy_macro_start + proxy_macro_number + ml.proxy_macro_end + ":set"
    # return the unprocessed line
    # ui.report_verbose("process_set_macro returning unprocessed line '{}'".format(line))
    return line

## ----------------------------------------------------

# process macro call if in line
def process_macro_call(line):
    macro_name = None
    line = line.strip()
    args = utils.extract_args(line, ml.run_macro_delimiters)
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
    args = utils.extract_args(line, ml.get_variable_delimiters)
    if len(args) > 0:
        variable_name = args[0]
        if variable_name in resolved:
            # replace the variable reference with the resolved value
            resolved_value = resolved[variable_name]
            #ui.report_verbose("process_get_variable replacing variable reference '{}' with '{}'".format(variable_name, resolved_value))
            return utils.replace_args(line, ml.get_variable_delimiters, resolved_value)
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
    args = utils.extract_args(line, ml.allocate_sequencer_delimiters)
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
        return utils.replace_args(line, ml.allocate_sequencer_delimiters, str(resolved_value))

    # return the unprocessed line
    # ui.report_verbose("process_allocate_sequencer returning unprocessed line '{}'".format(line))
    return line

## ----------------------------------------------------

def process_place_template(line):
    line = line.strip()
    if len(line) < 1:
        return ''

    # see if line has a template expqnsion
    args = utils.extract_args(line, ml.expand_template_delimiters)
    if len(args) > 0:
        template_name = args[0]
        # can only expand if there are no unresolved values
        if template_name in resolved:
            template_script = resolved[template_name]
            ui.report_verbose("process_place_template expanding template " + template_name)

            return utils.replace_args(line, ml.expand_template_delimiters, template_script)
    # return the unprocessed line
    # ui.report_verbose("process_place_template returning unprocessed line '{}'".format(line))
    return line    

## ----------------------------------------------------

def process_configure(line):
    line = line.strip()
    if len(line) < 1:
        return ''

    # see if line has a configuration shortcut
    if ml.configuration_marker in line:
        halves = line.split(ml.configuration_marker)
        if len(halves) == 2:
            halves[0] = halves[0].strip()
            halves[1] = halves[1].strip()

            if len(halves[0]) > 0 and len(halves[1]) > 0:
                return halves[0] + "," + halves[1] + ":cfg"
    return line


########################################################################
## main processing helpers
########################################################################

def is_command(_str):
    pattern = re.compile("^[a-zA-Z-]*$")
    return pattern.match(_str) != None

# join macros that span multiple lines
# into a single line
def consolidate_macros(script_lines):
    new_lines = []
    building_commands = ""
    for line in script_lines:
        if line.endswith(":set"):
            if len(building_commands) > 0:
                #ui.report_verbose("consolidate_macros built line1: " + building_commands)

                if building_commands.endswith(":set:"):
                    raise ValueError("Empty macro is not allowed: " + building_commands)

                new_lines.append(building_commands[:-1])
                building_commands = ""

        if is_command(line):
          if(len(line) > 3):
            raise ValueError("Unknown command '" + line + "' cannot be programmed")
        building_commands += line + ":"

    if len(building_commands) > 0:
        #ui.report_verbose("consolidate_macros built line2: " + building_commands)
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
        ":0,0,0:" : ":",
        ",0,0:" : ":",
        ",0:" : ":",
        # remove spaces
        " " : "",
    }
    return do_clean_ups(script_lines, clean_ups)


########################################################################
## macro number assignment routines
########################################################################

# expects a proxy macro number in the form '4'
def is_macro_number_in_use(macro_number):
    for value in macros.values():
        if ml.proxy_macro_start in str(value):
            value = int(value[len(ml.proxy_macro_start):-len(ml.proxy_macro_end)])

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
    names = unresolved.keys()
    names.sort()
    for name in names:
        if unresolved[name] == None:
            new_macro_number = get_next_macro_number()

            # proxy numbers will be in the form '10' 
            ui.report_verbose("proxy_macro_numbers assigning proxy macro #" + str(new_macro_number) + " for macro: " + name)
            proxy_macro_value = ml.proxy_macro_start + ("0" + str(new_macro_number))[-2:] + ml.proxy_macro_end

            resolve_unresolved(name, new_macro_number)
            set_resolved(name, proxy_macro_value)
            set_macro(name, proxy_macro_value)
    remove_resolved()

## ----------------------------------------------------

def assign_final_macro_number(line):
    global saved_bad_script

    start, end = utils.locate_delimiters(line, ml.proxy_macro_delimiters)
    # only process lines starting with proxy macro numbers
    if start != 0:
        return line

    proxy_macro_number = int(utils.extract_contents(line, ml.proxy_macro_delimiters))

    # temporarily replace this macro's unresolved references 
    # with a memory macro to use to measure the size
    # use macro #0 to have the most available space
    test_macro = utils.replace_args(line, ml.proxy_macro_delimiters, "0")

    # replace remaining references with #1 to ensure args are stored
    while ml.proxy_macro_start in test_macro:
        test_macro = utils.replace_args(test_macro, ml.proxy_macro_delimiters, "1")

    # send to the device and check for consumed macro bytes
    if len(test_macro) > max_string_length:
        raise ValueError("Macro being tested exceeds char buffer size")
        # todo: handle this automatically

    bytes_used = 0
    tries = 3
    led_command.stop_all()
    while bytes_used == 0 and tries > 0:
        ui.report_verbose("assign_final_macro_number measuring proxy macro #" + str(proxy_macro_number) + " on device")
        ui.report_verbose_alt2("macro being tested: " + test_macro)

        bytes_used = led_command.command_int(test_macro)
        ui.report_verbose_alt("assign_final_macro_number reported size: " + str(bytes_used) + " bytes")
        tries -= 1

    if bytes_used == 0:
        raise ValueError("Unable to measure macro: '" + test_macro + "'")

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

            # open block of macro numbers found
            break

        final_macro_number = potential_macro_number

    ui.report_verbose_alt("assign_final_macro_number assigning final macro #" + str(final_macro_number) + " for proxy #" + str(proxy_macro_number))
    ui.report_progress()
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
    return utils.replace_args(line, ml.proxy_macro_delimiters, str(final_macro_number)) 

## ----------------------------------------------------

def process_finalized_macro_numbers_pass(script_lines):
    new_lines = []
    for line in script_lines:
        args = utils.extract_args(line, ml.proxy_macro_delimiters)
        if len(args) == 1:
            proxy_macro_number = int(args[0])

            if proxy_macro_number in get_final_macro_numbers():
                final_macro_number = get_final_macro_numbers()[proxy_macro_number]
                new_line = utils.replace_args(line, ml.proxy_macro_delimiters, final_macro_number)
                new_lines.append(new_line)
            else:
                new_lines.append(line)
        else:
            new_lines.append(line)
    return new_lines

## ----------------------------------------------------

def process_finalized_macro_numbers(script_lines):
    processed_lines = script_lines
    pass_num = 0
    while True:
        prev_lines = processed_lines
        ui.report_progress()
        processed_lines = process_finalized_macro_numbers_pass(processed_lines)
        pass_num += 1
        ui.report_verbose_script(processed_lines, "script after finalizing macro numbers pass " + str(pass_num)) 

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
        prev_lines = processed_lines
        processed_lines = assign_final_macro_numbers_pass_one(processed_lines)
        ui.report_verbose_script(processed_lines, "script after final macro number assignment pass #1")

        processed_lines = assign_final_macro_numbers_pass_two(processed_lines)
        ui.report_verbose_script(processed_lines, "script after final macro number assignment pass #2")

        if processed_lines == prev_lines:
            # no more processing is possible
            return processed_lines


########################################################################
## line processing helpers
########################################################################

def line_has_both_delimiters(line, delimiters):
    if len(line) < 1:
        return False

    for start_delimiter in delimiters:
        end_delimiter = delimiters[start_delimiter]
        if start_delimiter in line and end_delimiter in line:
            return True
    return False

def line_has_starting_delimiter(line, delimiters):
    start_delimiter = delimiters.keys()[0]
    return len(line) > 0 and start_delimiter in line

def line_has_unresolved_variables(line):
    return line_has_both_delimiters(line, ml.get_variable_delimiters)

def line_has_python_expression(line):
    return line_has_starting_delimiter(line, ml.python_delimiters)

def line_has_macro_marker(line):
    return line_has_both_delimiters(line, ml.macro_marker_delimiters)

def line_has_template_marker(line):
    return line_has_both_delimiters(line, ml.template_marker_delimiters)

def line_has_meta_template_marker(line):
    return line_has_both_delimiters(line, ml.meta_template_delimiters)

def line_has_multi_macro_marker(line):
    return line_has_both_delimiters(line, ml.multi_macro_delimiters)

def line_has_sequence_marker(line):
    return line_has_both_delimiters(line, ml.allocate_sequencer_delimiters)

def line_has_unresolved(line):
    return line_has_unresolved_variables(line) or line_has_python_expression(line) or line_has_template_marker(line) or line_has_macro_marker(line) or line_has_sequence_marker(line)

def line_has_unresolved_for_python_evaluation(line):
    return line_has_unresolved_variables(line) or line_has_python_expression(line) or line_has_template_marker(line) or line_has_sequence_marker(line)

def line_has_unresolved_for_include_rewrite(line):
    return line_has_template_marker(line) or line_has_multi_macro_marker(line)

def line_has_unresolved_for_include_prefixing(line):
    return line_has_python_expression(line)

########################################################################
## line maniupation routines
########################################################################

def replace_all_variables(line):
    orig_line = line
    runaway_line_length = max_string_length * 4
    max_times = 100
    while True:
        max_times -= 1
        prev_line = line
        line = process_get_variable(line)

        if len(line) > runaway_line_length or max_times == 0:
            raise ValueError("Infinite recursion on line: " + orig_line)

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

def is_known_resolved(name):
    name = name.strip()
    return name in resolved

def is_known_unresolved(name):
    name = name.strip()
    return name in unresolved

# return true if resolved value is a mutable preset and can be changed
# raise ValueError if resolved value is immutable value being changed
def immutable_resolved_value(variable_name, variable_value):
    if allow_mutability:
        return False

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

# inserts resolved values for compiler use
def add_default_resolved():
    default_resolved = {
        "INCLUDED" : False
    }
    for key in default_resolved:
        #ui.report_verbose("setting default resolved value " + key + "=" + str(default_resolved[key]))
        set_resolved(key, default_resolved[key])

########################################################################
## general table management
########################################################################

def set_macro(name, value):
    #global macros
    macros[name] = value

def macro_number_in_use(number):
    return number in macros.values()

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



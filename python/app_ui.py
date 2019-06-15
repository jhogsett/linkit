import sys
import terminal_colors as tc
import utils

global verbose_mode
verbose_mode = False

# these can only be set, not cleared, in case of multiple usages
def begin(verbose, no_colors=False):
    global verbose_mode
    if verbose:
        verbose_mode = verbose
    if no_colors:
        tc.begin(True);

def report_error(message):
    print tc.red(message)

def report_error_alt(message):
    print tc.yellow(message)

def report_info(message):
    print tc.cyan(message)

def report_success(message):
    print tc.green(message)

def report_info_header(message):
    print tc.cyan(message),

def report_info_alt(message):
    print tc.green(message)

def report_info_alt2(message):
    print tc.yellow(message)

def report_info_alt3(message):
    print tc.white(message)

def report_warn(message):
    print tc.yellow(message)

def report_separator():
    print

def report_verbose(message=""):
    if verbose_mode:
        print tc.green(message)

def report_verbose_alt(message=""):
    if verbose_mode:
        print tc.yellow(message)

def report_verbose_alt2(message=""):
    if verbose_mode:
        print tc.red(message)

def report_verbose_alt3(message=""):
    if verbose_mode:
        print tc.white(message)

def verbose_entry(key, value):
    if verbose_mode:
        print tc.green(key) + ": " + tc.white(str(value))        

def report_verbose_header(message="", char='-', width=60, spacer=" "):
    if verbose_mode:
        remaining_chars = width - len(message) - 2
        header = char * (remaining_chars / 2)
        report_verbose(header + spacer + message + spacer + header)

def write_line(text):
    print text

def write(text):
    sys.stdout.write(text)
    sys.stdout.flush()

def report_header():
    print "\n----------------------------------------------------"

def report_footer():
    print

def info_entry(key, value):
    print tc.cyan(key) + ": " + tc.green(str(value))

def info_entry_alt(key, value):
    print tc.cyan(key) + ": " + tc.yellow(str(value))

# deprecated
def intro_entry(key, value):
    return tc.cyan(key) + ": " + tc.green(str(value))

def app_description(description):
    print tc.magenta("\n" + description + "\n")

def print_script(script):
    for line in script:
        report_error(line)
    report_error()

def report_verbose_script(script, message):
    if not verbose_mode:
        return
    line_count = len(script)
    byte_count = 0
    for line in script:
        byte_count += len(line)
    hash = utils.hash_object(script)
    count_message = " (lines: " + str(line_count) + ", bytes: " + str(byte_count) + ", hash: " + hash + ")"
    report_verbose_alt(message + count_message)
    for line in script:
        report_verbose(line)
    report_verbose()

def report_progress():
        if not verbose_mode:
                write(tc.green("."))


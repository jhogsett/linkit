import sys
import terminal_colors as tc

global verbose_mode
verbose_mode = False

def begin(verbose):
    global verbose_mode
    verbose_mode = verbose

def report_error(message):
    print tc.red(message)

def report_info(message):
    print tc.cyan(message)

def report_warn(message):
    print tc.yellow(message)

def report_verbose(message=""):
    if verbose_mode:
        print tc.green(message)

def report_verbose_alt(message=""):
    if verbose_mode:
        print tc.blue(message)

def verbose_entry(key, value):
    if verbose_mode:
        print tc.green(key) + ": " + tc.white(str(value))        

def write(text):
    sys.stdout.write(text)
    sys.stdout.flush()

def report_header():
    print "\n----------------------------------------------------"

def report_footer():
    print

def info_entry(key, value):
    print tc.cyan(key) + ": " + tc.green(str(value))

# deprecated
def intro_entry(key, value):
    return tc.cyan(key) + ": " + tc.green(str(value))

def app_description(description):
    print tc.magenta("\n" + description + "\n")

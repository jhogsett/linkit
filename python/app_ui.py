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
    print tc.white(message)

def report_verbose(message=""):
    if verbose_mode:
        print tc.yellow(message)

def write(text):
  sys.stdout.write(text)
  sys.stdout.flush()

def report_header():
    print "\n----------------------------------------------------"

def report_footer():
    print

def intro_entry(key, value):
    return tc.white(key) + ": " + tc.green(str(value))

def app_description(description):
    print tc.magenta("\n" + description + "\n")


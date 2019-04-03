import app_ui as ui
import terminal_colors as tc
import led_command as lc
import utils

def verify_programming(compiled_script):
  script_ok = True
  for compiled_line in compiled_script:
    macro_number = int(compiled_line.split(":")[0])
    programmed_line, carry_over_macro = lc.get_macro(macro_number)
    if programmed_line != compiled_line:
      script_ok = False
      print
      ui.report_error("Macro doesn't match:")
      print tc.green("Expected: " + compiled_line)
      print tc.red("     Got: " + programmed_line)
      print
    ui.write(tc.green('.'))
  print
  return script_ok

def print_script(script_lines):
  for script_text in script_lines:
    ui.report_warn(script_text)

def print_table(description, table):
    print
    ui.report_info_alt("------------------------------------------------------")
    ui.report_info_alt(description + ":")
    ui.report_info_alt("------------------------------------------------------")
    keys = table.keys()
    values = table.values()
    keys_width = utils.get_list_width(keys)
    values_width = utils.get_list_width(values)

    for key in sorted(table.iterkeys()):
        key_len = len(str(key))
        key_diff = keys_width - key_len
        value = table[key]
        filler = " " * key_diff
        key_title = filler + str(key)
        if type(value) is list:
          ui.report_info_alt(key_title + ":")
          print_script(value)
        else:
          ui.info_entry_alt(key_title, str(value))
    print

def print_device_macros():
  macros = lc.get_macros()
  ui.report_info("Macros on device:")
  for macro in macros:
    ui.report_info_alt(macro)



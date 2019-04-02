import app_ui as ui
import terminal_colors as tc
import led_command as lc

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


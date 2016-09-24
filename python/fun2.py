import serial 
import time
s = None

inter_command_delay = 0.02

def command(cmd_text):                                                                   
  s.write((cmd_text + '|').encode())                                                     
  time.sleep(inter_command_delay)    

def setup(): 
  global s 
  s = serial.Serial("/dev/ttyS0", 57600) 
  command("fade")
  time.sleep(4)

def run(): 
  command("pause")

  for y in range(0, 12):
    command("red|blink1")
    command("orange|blink2")
    command("yellow|blink3")
    command("green|blink4")
    command("blue|blink5")
    command("purple|blink6")

  command("continue")

if __name__ == '__main__': 
  setup()
  run() 

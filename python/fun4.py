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

  for y in range(0, 6):
    command("red|blink1")
  for y in range(0, 6):
    command("orange|blink2")
  for y in range(0, 6):
    command("yellow|blink3")
  for y in range(0, 6):
    command("green|blink4")
  for y in range(0, 6):
    command("blue|blink5")
  for y in range(0, 6):
    command("purple|blink6")

  command("mirror");
  command("continue")

if __name__ == '__main__': 
  setup()
  run() 

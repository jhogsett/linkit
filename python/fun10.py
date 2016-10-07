import serial 
import time
import random

s = None

inter_command_delay = 0.01

def command(cmd_text):                                                                   
  s.write((cmd_text + '|').encode())                                                     
  time.sleep(inter_command_delay)    

def setup(): 
  global s 
  s = serial.Serial("/dev/ttyS0", 57600) 
#  command("fade")
#  time.sleep(2)
  command("erase")

blink_map = {
  0 : "blink1",
  1 : "blink2",
  2 : "blink3",
  3 : "blink4",
  4 : "blink5",
  5 : "blink6"
}

def run(): 
  while True:
    command("pause")
    command("pause")

    command("purple")
    command("blink6")
    command("23|repeat")

    command("blue")
    command("blink5")
    command("23|repeat")

    command("green")
    command("blink4")
    command("23|repeat")

    command("yellow")
    command("blink3")
    command("23|repeat")

    command("orange")
    command("blink2")
    command("23|repeat")

    command("red")
    command("blink1")
    command("23|repeat")

    command("continue")

    exit()

if __name__ == '__main__': 
  setup()
  run() 

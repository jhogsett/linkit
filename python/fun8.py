import serial 
import time
import random

s = None

inter_command_delay = 0.0

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

    for x in range(0, 8):
      choice = random.randrange(0, 6)
      for y in range(0, 6):
        blink = blink_map[choice]
        command("random")
        command(blink)
      for y in range(0, 2):          
        command('random|static')

    command("continue")
    time.sleep(10.0)

if __name__ == '__main__': 
  setup()
  run() 

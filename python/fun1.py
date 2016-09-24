import serial 
import time
s = None

def command(cmd_text):
  s.write((cmd_text + "|").encode())  
  time.sleep(0.02)

def setup(): 
  global s 
  s = serial.Serial("/dev/ttyS0", 57600) 
  command("fade")

def loop(): 
  command("pause")
  for x in range(0, 6):
    for y in range(0, 12):
      command("red|blink1")
      command("blue|blink3")

  command("continue")
  exit()

if __name__ == '__main__': 
  setup() 
  while True: 
    loop()

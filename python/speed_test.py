import serial 
import time
s = None
def setup(): 
  global s 
  s = serial.Serial("/dev/ttyS0", 57600) 
  command("fade");

def command(cmd_text):
  s.write((cmd_text + '|').encode())
  time.sleep(0.1)                                                                                                                                                                                          

def loop():
  for x in range(0, 144):
   # command("pause")
    for y in range(0, x):
      command("red")
    for y in range(x, 144):
      command("blue") 
   # command("continue|flush") 

if __name__ == '__main__': 
  setup() 
  while True: 
    loop()

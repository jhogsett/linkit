import serial 
import time
import random

s = None
def setup(): 
  global s 
  s = serial.Serial("/dev/ttyS0", 57600) 
  s.write("erase\0".encode())

def loop(): 
  r = random.randint(0, 8)

  switcher = {
    0: "red\0",
    1: "orange\0",
    2: "yellow\0",
    3: "green\0",              
    4: "blue\0",              
    5: "purple\0",              
    6: "cyan\0",              
    7: "magenta\0",              
    8: "pink\0",              
    9: "white\0",              
    10: "black\0",              
  }

  color = switcher.get(r, "erase\0")
  s.write(color.encode()) 


  time.sleep(0.005)

if __name__ == '__main__': 
  setup() 
  while True: 
    loop()

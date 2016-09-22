import serial 
import time
s = None
def setup(): 
  global s 
  s = serial.Serial("/dev/ttyS0", 57600) 
  s.write("erase|".encode())

def loop(): 
  color = raw_input('color: ')
  s.write((color + '|').encode()) 

if __name__ == '__main__': 
  setup() 
  while True: 
    loop()

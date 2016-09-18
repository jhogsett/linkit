import serial 
import time
s = None
def setup(): 
  global s 
  s = serial.Serial("/dev/ttyS0", 57600) 
  s.write("erase\0".encode())

def loop(): 
  s.write("red\0".encode()) 
  time.sleep(1) 

  s.write("green\0".encode()) 
  time.sleep(1) 

  s.write("blue\0".encode())
  time.sleep(1)

  s.write("orange\0".encode())
  time.sleep(1)








if __name__ == '__main__': 
  setup() 
  while True: 
    loop()

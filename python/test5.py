import serial 
import time
import random
import requests

s = None
def setup(): 
  global s 
  s = serial.Serial("/dev/ttyS0", 57600) 
  s.write("erase\0".encode())

def loop(): 
  r = requests.get('https://www.random.org/integers/?num=1&min=0&max=10&col=1&base=10&format=plain&rnd=new') #, auth=('user', 'pass'))

#print r.status_code
#print r.headers['content-type']

  i = int(r.text)

#  r = random.randint(0, 8)

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

  color = switcher.get(i, "erase\0")
  s.write(color.encode()) 

#  time.sleep(0.01 + random.random() / 5.0)

  time.sleep(5)


if __name__ == '__main__': 
  setup() 
  while True: 
    loop()

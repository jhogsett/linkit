import serial 
import time
import requests
import json
import sys

token = sys.argv[1]
url = 'https://circleci.com/api/v1.1/recent-builds?circle-token=' + token + '&limit=72&offset=0'

s = None
def setup(): 
  global s 
  s = serial.Serial("/dev/ttyS0", 57600) 
  s.write("erase\0".encode())

def loop(): 
  r = requests.get(url)
  r = r.text.encode('utf-8')
  j = json.loads(r)

#print r.status_code
#print r.headers['content-type']

  switcher = {
    "failed": "red\0",
    "success": "green\0",
    "fixed": "blue\0",
    "no_tests": "white\0",              
    "retried": "orange\0", 
    "timedout":"cyan\0",
    "canceled":"yellow\0",
    "not_run":"black\0",
    "running":"pink\0",
    "queued":"magenta\0",
    "scheduled":"magenta\0",
    "not_running":"black\0",
    "infrastructure_fail":"cyan\0"             
  }

  for x in range(0, 72):
    st = j[71-x]['status'].encode('ascii', 'ignore')
    color = switcher.get(st, "pink\0")
    s.write(color.encode()) 
    time.sleep(0.01)

  time.sleep(60)

if __name__ == '__main__': 
  setup() 
  while True: 
    loop()

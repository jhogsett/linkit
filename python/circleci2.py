import serial 
import time
import requests
import json
import sys

token = sys.argv[1]
url = 'https://circleci.com/api/v1.1/recent-builds?circle-token=' + token + '&limit=18&offset=0'

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
    "finished": "green\0",
    "fixed": "blue\0",
    "no_tests": "white\0",              
    "retried": "orange\0", 
    "timedout":"red\0",
    "canceled":"yellow\0",
    "not_run":"white\0",
    "running":"pink\0",
    "queued":"magenta\0",
    "scheduled":"purple\0",
    "not_running":"white\0",
    "infrastructure_fail":"cyan\0"             
  }

  for x in range(0, 18):
    lc = j[17-x]['lifecycle'].encode('ascii', 'ignore')
    oc = j[17-x]['outcome'].encode('ascii', 'ignore')
    st = j[17-x]['status'].encode('ascii', 'ignore')

    color = switcher.get(lc, "black\0")
    s.write(color.encode()) 
    time.sleep(0.01)

    color = switcher.get(oc, "black\0")
    s.write(color.encode())
    time.sleep(0.01)

    color = switcher.get(st, "black\0")
    s.write(color.encode())
    time.sleep(0.01)

    color = "black\0"
    s.write(color.encode())
    time.sleep(0.01)
  time.sleep(60)

if __name__ == '__main__': 
  setup() 
  while True: 
    loop()

#!/usr/bin/python

import serial 
import time
import requests
import json
import sys
import logging
import os

script_path = os.getcwd()
log_path = "/root/dev/linkit/python/circleci.log"
print "logging to " + log_path
logging.basicConfig(filename=log_path, level=logging.INFO, format='%(asctime)s %(message)s')
logging.info("Circleci5.py started")

job_limit = 16
max_job = job_limit - 1
max_leds = job_limit * 4

token = sys.argv[1]
url = 'https://circleci.com/api/v1.1/recent-builds?circle-token=' + token + '&limit=' + str(job_limit) + '&offset=0'
request_frequency = 15

s = None

color_map = {                            
              "running": "ltblue:breathe",           
              "success": "green",                    
             "finished": "green",                 
                "fixed": "seafoam",                       
               "failed": "red",              
             "no_tests": "dkgray",                    
              "retried": "pink",                  
             "timedout": "yellow:blink",            
             "canceled": "orange",                   
              "not_run": "yellow",             
               "queued": "purple:breathe",                
            "scheduled": "magenta:breathe",           
          "not_running": "yellow",          
              "missing": "white",                    
               "spacer": "black",
         "masterfailed": "red:blink",
          "masterfixed": "green:blink",                       
  "infrastructure_fail": "pink:blink",
         "jerryrunning": "magenta:breathe"       
}   

def flush_input():
  s.read(s.inWaiting())

def wait_for_ack():
  while s.inWaiting() == 0:
    pass
  flush_input()

def command(cmd_text):
  s.write((cmd_text + ':').encode())   
  wait_for_ack()

def get_color_cmd(color_cmd_text):
  if color_cmd_text is None:
    return "missing"
  else:
    return color_map.get(color_cmd_text, "black")    

def color_command(color_cmd_text):                     
  color_cmd = get_color_cmd(color_cmd_text)
  command(color_cmd)

def color_command3(color_cmd_text):
  color_cmd = get_color_cmd(color_cmd_text)     
  for i in range(0, 3):
    command(color_cmd)                               

def spacer():                              
  color_command("spacer") 

def fix_missing(value):
  if value is None:
    return "missing"
  else:
    return value

build_keys = None 

def setup():
  global s
  global build_keys
  s = serial.Serial("/dev/ttyS0", 115200)
  build_keys = {}
  time.sleep(0.1)
  flush_input()
  command("::reset")

oldest_first = True

def translate_position(pos):
  if oldest_first:                                                                
    return (job_limit - 1) - pos                                                       
  else:                                                                           
    return pos 
 
def loop():
  global build_keys
  try:
    r = requests.get(url)
    r = r.text.encode('utf-8')
    j = json.loads(r)

    command("pause");

    insert_count = 0                                                                  
    for x in range(0, job_limit):                
      y = translate_position(x)
      key = j[y]['build_url']                                                         
      if not build_keys.has_key(key):                                                 
        insert_count += 4                                                             

    if insert_count > 0:
      command(str(max_leds - insert_count) + "," + str(insert_count) + ":pshifto")

    build_keys = {} 
    for x in range(0, job_limit):
      y = translate_position(x)                                                       
      key = j[y]['build_url']
      build_keys[key] = True
      st = fix_missing(j[y]['status'])
      lc = fix_missing(j[y]['lifecycle'])                     
      oc = fix_missing(j[y]['outcome'])      
      br = j[y]['branch']
      rp = j[y]['reponame']
      cn = fix_missing(j[y]['committer_name'])

      master = br == 'master'
      orders = rp == 'orders'
      orders_master = master and orders
      jh = cn == 'jerry'
      logging.info("st:" + st + " lc:" + lc + " oc:" + oc + " br:" + br + " rp:" + rp + " cn:" + cn)            

      if st == 'failed':
        if orders_master:
          color_command3('masterfailed')
        else: 
          color_command3('failed')
      elif st == 'success':
        color_command3('success')
      elif st == 'running':
        if jh:
          color_command3('jerryrunning') 
        else:
          color_command3('running')           
      elif st == 'fixed':
        if orders_master:
          color_command3('masterfixed')                                                                               
        else:
          color_command3('fixed')  
      elif oc == 'infrastructure_fail':
        color_command3('infrastructure_fail')             
      elif oc == 'no_tests':
        color_command3('no_tests')                                                                                                                                                               
      elif oc == 'timedout':
        color_command3('timedout')                                                                                                                                                               
      elif oc == 'canceled':
        color_command3('canceled')
      elif st == 'not_run':
        color_command3('not_run') 
      else:
          color_command(st)                
          color_command(oc)                            
          color_command(lc)                          
      spacer()  

    command("continue:flush")
    time.sleep(request_frequency)

  except requests.exceptions.ConnectionError:
    logging.error("Connection error - retrying")
    command("pause:blue:blink:flood:continue")
    time.sleep(15)
  except Exception:
    logging.error(sys.exc_info()[0])
    command("pause:yellow:blink:flood:continue")
    raise

if __name__ == '__main__': 
  setup() 
  while True:
    loop()
 

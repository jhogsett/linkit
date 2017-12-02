#!/usr/bin/python

import serial 
import time
import requests
import json
import sys
import logging
import os
import traceback
import sys

script_path = os.getcwd()
#log_path = script_path + "/circleci.log"
#print "logging to " + log_path

#logging.basicConfig(filename=log_path, level=logging.INFO, format='%(asctime)s %(message)s')
#logging.info("Circleci7.py started")

global visible_leds, job_size, gap_size
visible_leds = 72
job_size = 3
gap_size = 1

try:
  token = sys.argv[1]
except Exception:
  sys.exit("\nNo API key supplied!\n")

#global url
#url = 'https://circleci.com/api/v1.1/recent-builds?circle-token=' + token + '&limit=' + str(job_limit) + '&offset=0'

request_frequency = 15
s = None

color_map = {                            
              "running": "lbl:bre",           
              "success": "grn",                    
             "finished": "grn",                 
                "fixed": "sea",                       
               "failed": "red",              
             "no_tests": "dgr",                    
              "retried": "pnk",                  
             "timedout": "yel:bli",            
             "canceled": "org",                   
              "not_run": "gry",             
               "queued": "pur:bli",                
            "scheduled": "lgr:bre",           
          "not_running": "pur:bli",          
              "missing": "dgr",                    
               "spacer": "blk",
         "masterfailed": "red:bli",
          "masterfixed": "grn:bli",                       
  "infrastructure_fail": "yel:bli",
         "unrecognized": "blu:bla"       
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
    return color_map.get(color_cmd_text, "blk")    

def color_command(color_cmd_text):                     
  color_cmd = get_color_cmd(color_cmd_text)
  for i in range(0, job_size):                                                                                                  
    command(color_cmd)                                                                                                   

def spacer():                              
  if gap_size < 1:
    return
  color_cmd = get_color_cmd("spacer")
  for i in range(0, gap_size):                                                                                                  
    command(color_cmd)                                                                                                   

def fix_missing(value):
  if value is None:
    return "missing"
  else:
    return value

build_keys = None 

def setup():
  global s, build_keys, visible_leds, url, job_size, gap_size
  s = serial.Serial("/dev/ttyS0", 115200)
  build_keys = {}
  time.sleep(0.1)
  flush_input()
  command(":::")
  command(":::stp:stp")

  if len(sys.argv) > 2:
    command(sys.argv[2])

  if len(sys.argv) > 3: 
    visible_leds = int(sys.argv[3])

  if len(sys.argv) > 4:                                                                                                      
    job_size = int(sys.argv[4])  

  if len(sys.argv) > 5:                                                                                                      
    gap_size = int(sys.argv[5])  

oldest_first = True

def translate_position(pos):
  if oldest_first:                                                                
    return (get_job_limit() - 1) - pos                                                       
  else:                                                                           
    return pos 
 
def get_value(json, value_name):                                                                    
  try:                                                                                              
    return fix_missing(json[value_name])                                                                         
  except Exception:                                                                                 
    return "missing"                                                                                

def get_job_limit():
  return visible_leds / (job_size + gap_size)

def get_url():
  return 'https://circleci.com/api/v1.1/recent-builds?circle-token=' + token + '&limit=' + str(get_job_limit()) + '&offset=0' 
               
def loop():
  global build_keys
  try:
    r = requests.get(get_url())
    r = r.text.encode('utf-8')
    j = json.loads(r)

    command(":::");
    command(":::pau:pau");

    insert_count = 0                                                                  
    for x in range(0, get_job_limit()):                
      y = translate_position(x)
      key = get_value(j[y], 'build_url')                                                         
      if not build_keys.has_key(key):                                                 
        insert_count += 4                                                             

    #if insert_count > 0:
    #  command(str(visible_leds - insert_count) + "," + str(insert_count) + ":pso")

    build_keys = {} 
    for x in range(0, get_job_limit()):
      y = translate_position(x)                                                       
      key = get_value(j[y], 'build_url')
      build_keys[key] = True
      st = get_value(j[y], 'status')
      lc = get_value(j[y], 'lifecycle')                     
      oc = get_value(j[y], 'outcome')      
      br = get_value(j[y], 'branch')
      rp = get_value(j[y], 'reponame')
      cn = get_value(j[y], 'committer_name')

      master = br == 'master'
      orders = rp == 'orders'
      orders_master = master and orders
#      logging.info("st:" + st + " lc:" + lc + " oc:" + oc + " br:" + br + " rp:" + rp + " cn:" + cn)            

      if st == 'failed':
        if orders_master:
          color_command('masterfailed')
        else: 
          color_command('failed')
      elif st == 'success':
        color_command('success')
      elif st == 'running':
        color_command('running')           
      elif st == 'fixed':
        if orders_master:
          color_command('masterfixed')                                                                               
        else:
          color_command('fixed')  
      elif oc == 'infrastructure_fail':
        color_command('infrastructure_fail')             
      elif oc == 'no_tests':
        color_command('no_tests')                                                                                                                                                               
      elif oc == 'timedout':
        color_command('timedout')                                                                                                                                                               
      elif oc == 'canceled':
        color_command('canceled')
      elif st == 'not_run':
        color_command('not_run') 
      elif st == 'not_running':
        color_command('not_running')
      elif st == 'queued':
        color_command('queued');
      else:
          color_command('unrecognized')                
      spacer()  

    command("flu:1:cnt")
    time.sleep(request_frequency)

  except requests.exceptions.ConnectionError:
#    logging.error("Connection error - retrying")
    command(":::pau:dgr:bli:flo:cnt")
    time.sleep(15)
  except KeyboardInterrupt:
    sys.exit("\nExiting...\n")
  except Exception:
 #   logging.error(sys.exc_info()[0])
 #   logging.error(traceback.format_tb(sys.exc_info()[2]))
    command("pau:yel:bli:flo:cnt")
    raise

if __name__ == '__main__': 
  setup() 
  while True:
    loop()
 

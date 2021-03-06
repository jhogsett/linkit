#!/usr/bin/python

import serial 
import time
import requests
import json
import sys

job_limit = 16
max_job = job_limit - 1

token = sys.argv[1]
url = 'https://circleci.com/api/v1.1/recent-builds?circle-token=' + token + '&limit=' + str(job_limit) + '&offset=0'
request_frequency = 15

s = None

color_map = {                            
              "running": "ltblue|breathe",           
              "success": "green",                    
             "finished": "green",                 
                "fixed": "seafoam",                       
               "failed": "red",              
             "no_tests": "gray",                    
              "retried": "yellow",                  
             "timedout": "yellow|blink",            
             "canceled": "gray",                   
              "not_run": "gray",             
               "queued": "purple|breathe",                
            "scheduled": "purple",           
          "not_running": "pink",          
              "missing": "gray",                    
               "spacer": "black",
         "masterfailed": "red|blink",
          "masterfixed": "green|blink",                       
  "infrastructure_fail": "pink"        
}   

inter_command_delay = 0.02

def command(cmd_text):
  s.write((cmd_text + '|').encode())   
  time.sleep(inter_command_delay)                       

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

def setup():
  global s
  s = serial.Serial("/dev/ttyS0", 57600)
  command("erase")

def loop():
  r = requests.get(url)
  r = r.text.encode('utf-8')
  j = json.loads(r)

  command("pause");

  for x in range(0, job_limit):
    st = fix_missing(j[x]['status'])
    lc = fix_missing(j[x]['lifecycle'])                     
    oc = fix_missing(j[x]['outcome'])      
    br = j[x]['branch']
    rp = j[x]['reponame']
    master = br == 'master'
    orders = rp == 'orders'
    orders_master = master and orders
#    print "st:\t" + st + "\tlc:\t" + lc + "\toc:\t" + oc + "\tbr:\t" + br + "\trp:\t" + rp            

    if st == 'failed':
      if orders_master:
        color_command3('masterfailed')
      else: 
        color_command3('failed')
    elif st == 'success':
      color_command3('success')
    elif st == 'running':
      color_command3('running')           
    elif st == 'fixed':
      if orders_master:
        color_command3('masterfixed')                                                                               
      else:
        color_command3('fixed')  
    elif oc == 'infrastructure_fail':
      color_command3('infrastructure_fail')             
    else:
        color_command(st)                
        color_command(oc)                            
        color_command(lc)                          
    spacer()  

  command("continue");
  command("flush");
  time.sleep(request_frequency)

if __name__ == '__main__': 
  setup() 
  while True:
    loop()
 

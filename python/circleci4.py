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

switcher = {                            
  "failed": "red",                       
  "success": "green",                    
  "finished": "green",                 
  "fixed": "blue",                       
  "no_tests": "gray",                    
  "retried": "orange",                  
  "timedout":"yellow\0blink",            
  "canceled":"pink",                   
  "not_run":"gray",             
  "running":"ltblue\0breathe",               
  "queued":"magenta\0breathe",                
  "scheduled":"purple",           
  "not_running":"white\0blink",          
  "infrastructure_fail":"white\0blink",
  "missing":"gray",                    
  "spacer":"black",
  "masterfailed":"red\0blink"                       
}   

def setup(): 
  global s 
  s = serial.Serial("/dev/ttyS0", 57600) 
  s.write("erase\0".encode())

inter_command_delay = 0.01

def command(cmd_text):
  s.write((cmd_text + '\0').encode())   
  time.sleep(inter_command_delay)                       

def color_command(color_cmd_text):                     
  color_cmd = switcher.get(color_cmd_text, "black")  
  command(color_cmd)
                                          
def spacer():                              
  color_command("spacer") 

def loop():
  r = requests.get(url)
  r = r.text.encode('utf-8')
  j = json.loads(r)

  command("pause");

  for x in range(0, job_limit):
    st = j[max_job - x]['status']

#    print "st: " + st

    if st == 'failed':
      br = j[max_job - x]['branch']
      if br == 'master':
        color_command('masterfailed')
        color_command('masterfailed')
        color_command('masterfailed')
      else: 
        color_command('failed')
        color_command('failed')
        color_command('failed')
    elif st == 'success':
      color_command('success')
      color_command('success')
      color_command('success')
    elif st == 'running':
      color_command('running')           
      color_command('running')           
      color_command('running')  
    else:
      lc = j[max_job - x]['lifecycle']               
      oc = j[max_job - x]['outcome']  

      if lc is not None:
#        print "lc: " + lc

        lc = lc.encode('ascii', 'ignore')  
        color_command(lc)
      else:
        color_command("missing")

      if oc is not None:                                  
#        print "oc: " + oc

        oc = oc.encode('ascii', 'ignore')                                                                                                                                                                    
        color_command(oc)                            
      else:                                            
        color_command("missing") 
      
      if st is not None:
        st = st.encode('ascii', 'ignore')                                                                                                                                                                    
        color_command(st)
      else:                                            
        color_command("missing") 
    
    spacer()  
#    print ""

  command("continue");
  command("flush");

  time.sleep(request_frequency)

if __name__ == '__main__': 
  setup() 
  while True:
    loop()
 

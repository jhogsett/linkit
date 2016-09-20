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
  "not_run":"gray\0blink",             
  "running":"ltblue\0breathe",               
  "queued":"magenta\0breathe",                
  "scheduled":"purple",           
  "not_running":"white\0blink",          
  "infrastructure_fail":"white\0blink",
  "missing":"ltblue\0blink",                    
  "spacer":"black"                       
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

    lc = j[max_job - x]['lifecycle']
    oc = j[max_job - x]['outcome']
    st = j[max_job - x]['status']

    if lc is not None:
      lc = lc.encode('ascii', 'ignore')  
      color_command(lc)
    else:
      color_command("missing")

    if oc is not None:                                  
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

  command("continue");
  command("flush");

  time.sleep(request_frequency)

if __name__ == '__main__': 
  setup() 
  while True:
    loop()
 

import serial 
import time
import requests
import json
import sys

job_limit = 16
max_job = job_limit - 1

token = sys.argv[1]
url = 'https://circleci.com/api/v1.1/recent-builds?circle-token=' + token + '&limit=' + str(job_limit) + '&offset=0'

s = None

switcher = {                            
  "failed": "red",                       
  "success": "green",                    
  "finished": "green",                 
  "fixed": "blue",                       
  "no_tests": "gray",                    
  "retried": "pink",                  
  "timedout":"yellow\0blink",            
  "canceled":"orange",                   
  "not_run":"ltblue\0blink",             
  "running":"green\0blink",               
  "queued":"ltgreen\0blink",                
  "scheduled":"purple\0blink",           
  "not_running":"white\0blink",          
  "infrastructure_fail":"red\0blink",
  "none":"green\0blink",                    
  "spacer":"black"                       
}   

def setup(): 
  global s 
  s = serial.Serial("/dev/ttyS0", 57600) 
  s.write("erase\0".encode())

def command(cmd_text):                     
  color = switcher.get(cmd_text, "black")  
  s.write((color + '\0').encode())         
  time.sleep(0.01)  
                                           
def spacer():                              
  command("spacer") 

def loop():
  r = requests.get(url)
  r = r.text.encode('utf-8')
  j = json.loads(r)

#print r.status_code
#print r.headers['content-type']

  for x in range(0, job_limit):
    lc = j[max_job - x]['lifecycle']
    oc = j[max_job - x]['outcome']
    st = j[max_job - x]['status']

    if lc is not None:
      lc = lc.encode('ascii', 'ignore')  
      command(lc)
    else:
      command("none")

    if oc is not None:                                  
      oc = oc.encode('ascii', 'ignore')                                                                                                                                                                    
      command(oc)                            
    else:                                            
      command("none") 
      
    if st is not None:
      st = st.encode('ascii', 'ignore')                                                                                                                                                                    
      command(st)
    else:                                            
      command("none") 
    
    spacer()  

  s.write("flush\0".encode())                                                                   
  time.sleep(60)

if __name__ == '__main__': 
  setup() 
  while True: 
    loop()

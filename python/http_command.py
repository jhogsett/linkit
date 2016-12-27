#!/usr/bin/python                   

import SocketServer
from BaseHTTPServer import BaseHTTPRequestHandler
import urlparse
import serial                       
import time                         
import sys  
from subprocess import call
import os
from socket import error as socket_error
import os.path

global webpage, base_path, last_run

last_run = ''

if len(sys.argv) > 2:
  base_path = sys.argv[2] + '/'
else:
  base_path = os.getcwd() + '/'

if len(sys.argv) > 1:
  webpage = sys.argv[1]
else:
  webpage = base_path + 'http_command.html'

global s
s = None                            

def wait_for_ack():      
  while s.inWaiting() <= 0:
    pass                   
  s.read(s.inWaiting())    
                           
def command(cmd_text):   
  s.write((cmd_text + ':').encode())
  wait_for_ack()   

class Handler(BaseHTTPRequestHandler):
  global last_run

  def run_app(self, app):
    global last_run    
    if app != 'stop':                      
      run = base_path + app + ' &'         
      self.log('running: ' + run)               
      call(run, shell=True)  
      last_run = app
    else:
      last_run = ''
  
  def kill_last_app(self):
    global last_run                       
    if last_run != '':                          
      self.log('killing: ' + last_run)          
      call('killall ' + last_run, shell=True) 
      last_run = ''

  def log(self, message):
    print
    print '#' * 80
    print '## ' + message 
    print '#' * 80
    print 

  def serve_page(self, page):
    filename, file_ext = os.path.splitext(page)

    self.send_response(200)
    if file_ext == '.html':
      content_type = "text/html"                     
    elif file_ext == '.css':
      content_type = "text/css"                     
    elif file_ext == '.js':
      content_type = "application/javascript"
    else:
      content_type = "text/plain"                     
    self.send_header("Content-type", content_type)
    self.end_headers()  

    f = open(page, 'r')                       
    self.wfile.write(f.read())                   
    f.close  
    self.wfile.close() 

  def do_GET(self):
    req = urlparse.urlparse(self.path)
 
    if req.path == '/command':
      args = urlparse.parse_qs(req.query)            

      if 'cmd' in args:
        command(":::pause")
        for cmd in args['cmd']:
          command(cmd)           
        command("flush:continue")

      if 'run' in args:
        self.kill_last_app() 
        self.run_app(args['run'][0])

      if 'sys' in args:
        for sys in args['sys']:
          self.log('shell command: ' + sys)
          call(sys, shell=True)

      # serve main page
      self.serve_page(webpage)

    elif os.path.isfile(base_path + req.path):
      page = base_path + req.path 
      #self.log('serving page: ' + page)
      self.serve_page(page)
    else:
      self.send_response(404)
      self.send_header("Content-type", "text/html")               
      self.end_headers()                             
      self.wfile.write("<html><body><h1>404 Not Found</h1></body></html>")
      self.wfile.close()                             

s = serial.Serial("/dev/ttyS0", 115200)

while(True):
  try:
    httpd = SocketServer.TCPServer(("", 8080), Handler)
  except socket_error,e:
    print "Error: " + str(e) + " - retrying"
    time.sleep(5)
    continue
  break 

print "Listening..."

try:
  httpd.serve_forever()
except KeyboardInterrupt:                                               
  sys.exit("\nExiting...\n") 
finally:
  if last_run != '':                         
    self.log('killing: ' + last_run)            
    call('killall ' + last_run, shell=True)     

#if __name__ == '__main__': 
#  setup() 
#  while True: 
#    loop()


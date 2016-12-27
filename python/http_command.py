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

  def serve_page(self, page):
    f = open(page, 'r')                       
    self.wfile.write(f.read())                   
    f.close  
    self.wfile.close() 

  def do_GET(self):
    req = urlparse.urlparse(self.path)

    if req.path == '/command':
      self.send_response(200)
      self.send_header("Content-type", "text/html")
      self.end_headers()
      args = urlparse.parse_qs(req.query)            

      if 'cmd' in args:
        command("::pause")
        for cmd in args['cmd']:
          command(cmd)           
        command("flush:continue")

      if 'run' in args:
        if last_run != '':
          print 'killing: ' + last_run
          call('killall ' + last_run, shell=True)
        last_run = args['run'][0] 
        run = base_path + last_run
        print 'running: ' + run
        call(run, shell=True)

      if 'sys' in args:
        for sys in args['sys']:
          print 'shell command: ' + sys
          call(sys, shell=True)

      self.serve_page(webpage)

#      f = open(webpage, 'r')                    
#      self.wfile.write(f.read())  
#      f.close                     
#      self.wfile.close()

    elif os.path.isfile(base_path + req.path):
      self.send_response(200)              
      self.send_header("Content-type", "text/css")
      self.end_headers()                           

      page = base_path + req.path 
      print 'serving page: ' + page
      self.serve_page(page)
    else:
      self.send_response(404)
      self.send_header("Content-type", "text/html")               
      self.end_headers()                             
      self.wfile.write("<html><body><h1>Not Found</h1></body></html>")
      self.wfile.close()                             

s = serial.Serial("/dev/ttyS0", 115200)

while(True):
  try:
    httpd = SocketServer.TCPServer(("", 8080), Handler)
  except socket_error,e:
    print "Error: " + str(e) + " - retrying"
    time.sleep(5)
    continue
  break; 

print "Listening..."
httpd.serve_forever()

#if __name__ == '__main__': 
#  setup() 
#  while True: 
#    loop()


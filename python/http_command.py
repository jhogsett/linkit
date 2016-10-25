#!/usr/bin/python                   

import SocketServer
from BaseHTTPServer import BaseHTTPRequestHandler
import urlparse
import serial                       
import time                         
import sys  

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
  def do_GET(self):
    req = urlparse.urlparse(self.path)

    if req.path == '/command':
      self.send_response(200)
      self.send_header("Content-type", "text/plain")
      self.end_headers()
      args = urlparse.parse_qs(req.query)            

      #self.wfile.write("Received: " + self.path)
      #self.wfile.write("\nQuery: " + req.query)

      print args

      for cmd in args['cmd']:
        self.wfile.write("Sending command: " + cmd + "... ") 
        self.wfile.flush()
        command(cmd)           
        self.wfile.write("sent\n")

      self.wfile.close()

    else:
      self.send_response(404)
      self.send_header("Content-type", "text/html")               
      self.end_headers()                             
      self.wfile.write("<html><body><h1>Not Found</h1></body></html>")
      self.wfile.close()                             

s = serial.Serial("/dev/ttyS0", 115200)
httpd = SocketServer.TCPServer(("", 8080), Handler)
print "Listening..."
httpd.serve_forever()

#if __name__ == '__main__': 
#  setup() 
#  while True: 
#    loop()


#!/usr/bin/python                   

import SocketServer
from BaseHTTPServer import BaseHTTPRequestHandler
import urlparse
import serial                       
import time                         
import sys  
from subprocess import call
import os
import socket
from socket import error as socket_error
import os.path

global webpage, base_path, last_run, host_name, host_ip

last_run = ''
last_run_full = ''
host_name = socket.getfqdn(socket.gethostname())
host_ip = socket.gethostbyname(socket.gethostname())

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
  global last_run, last_run_full, host_name, host_ip

  def run_app(self, app, track=True):
    global last_run, last_run_full    
    if app != 'stop':                      
      run = base_path + app + ' &'         
      self.log('running: ' + run)               
      call(run, shell=True)
      if track:
        last_run_full = app
        last_run = app.split()[0]
      else:
        last_run_full = ''
        last_run = ''
    else:
      last_run_full = ''
      last_run = ''
  
  def kill_last_app(self):
    global last_run, last_run_full                       
    if last_run != '':                          
      self.log('killing: ' + last_run)          
      call('killall -9 ' + last_run, shell=True) 
      last_run_full = ''
      last_run = ''

  def log(self, message):
    print
    print '#' * 80
    print '## ' + message 
    print '#' * 80
    print 

  def serve_page(self, page):
    global last_run, last_run_full, host_name, host_ip

    filename, file_ext = os.path.splitext(page)

    self.send_response(200)
    if file_ext == '.html':
      content_type = "text/html"                     
    elif file_ext == '.css':
      content_type = "text/css"                     
    elif file_ext == '.js':
      content_type = "application/javascript"
    elif file_ext == '.ico':
      content_type = "image/x-icon"
    elif file_ext == '.png':
      content_type = "image/png"
    else:
      content_type = "application/octet-stream"                     
    self.send_header("Content-type", content_type)
    self.end_headers()  

    if last_run != '':
      banner = """
<div class="well well-sm clearfix">
  <div class="col-xs-9"><i class="fa fa-circle-o-notch fa-spin fa-fw"></i> %(last_run)s</div>
  <div class="col-xs-3"><a class="btn btn-link btn-xs pull-right" role="button" href="/command?run=stop"><span class="glyphicon glyphicon-remove-sign"></span></a></div>
</div>
      """ % globals()
    else:
      banner = ''

    footer = """
<div class="small text-center">
  %(host_name)s %(host_ip)s
</div>
<div class="small text-center">                                                                                                                                         
  %(last_run_full)s                                                                                                                                             
</div>                                                                                                                                                                  
    """ % globals()

    f = open(page, 'r')                       
    self.wfile.write(f.read().replace('<!-- banner -->', banner).replace('<!-- footer -->', footer))
                   
    f.close  
    self.wfile.close() 

  def handle_commands(self, commands):
    command(":::pause")
    for cmd in commands:
      command(cmd)
    command("flush:continue")

  def do_GET(self):
    req = urlparse.urlparse(self.path)
 
    if req.path == '/command':
      args = urlparse.parse_qs(req.query)            

      if 'cmd' in args:

        # if an app is running, stop it first
        if last_run != '':
          to_run = last_run_full
          self.kill_last_app();
          self.handle_commands(args['cmd'])
          self.run_app(to_run)
        else:
          self.handle_commands(args['cmd'])

      if 'run' in args:
        self.kill_last_app() 
        self.run_app(args['run'][0])

      if 'runonce' in args:
        self.run_app(args['runonce'][0], False)

      if 'sys' in args:
        for sys in args['sys']:
          self.log('shell command: ' + sys)
          call(sys, shell=True)

      # serve main page
      self.serve_page(webpage)

    elif os.path.isfile(base_path + req.path):
      page = base_path + req.path 
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
    print 'killing: ' + last_run            
    call('killall ' + last_run, shell=True)     

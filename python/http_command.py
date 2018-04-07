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
import datetime
import led_command as lc
import argparse
import app_ui as ui

global httpd, webpage, base_path, last_run, host_name, host_ip, app_description, verbose_mode, debug_mode, num_leds, macro_count, programs, macro_run_number, retry_wait

last_run = ''
last_run_full = ''
host_name = socket.getfqdn(socket.gethostname())
host_ip = socket.gethostbyname(socket.gethostname())
app_description = None
verbose_mode = None
debug_mode = None
num_leds = None
retry_wait = None

#if len(sys.argv) > 2:
#  base_path = sys.argv[2] + '/'
#else:
#  base_path = os.getcwd() + '/'

#if len(sys.argv) > 1:
#  webpage = sys.argv[1]
#else:
#  webpage = base_path + 'http_command.html'

def get_options():
  global verbose_mode, debug_mode, retry_wait, base_path, webpage
  parser = argparse.ArgumentParser(description=app_description)
  parser.add_argument("page", metavar="P", nargs="?", help="path to web page")
  parser.add_argument("path", metavar="R", nargs="?", help="root path for files")
  parser.add_argument("-r", "--retry", type=int, dest="retry", default=10, help='retry wait (secs) (10)')
  parser.add_argument("-v", "--verbose", dest="verbose", action="store_true", help="display verbose info (False)")
  parser.add_argument("-d", "--debug", dest="debug", action="store_true", help="display verbose info (False)")
  args = parser.parse_args()
  retry_wait = args.retry
  verbose_mode = args.verbose
  debug_mode = args.debug

  base_path = args.path
  if base_path == None:
    base_path = os.getcwd() + '/'

  webpage = args.page
  if webpage == None:
    webpage = base_path + 'http_command.html'

def validate_options():
    errors = False
    return not errors

def initialize():
  global app_description, num_leds
  app_description = "Apollo Lighting System - HTTP Commander v.2.0 4-0-2018"
  get_options()
  if not validate_options():
    sys.exit("\nExiting...\n")
  lc.begin(verbose_mode)
  ui.begin(verbose_mode)
  lc.attention()
  lc.stop_all()
  num_leds = lc.get_num_leds()
  lc.command("cnt")

def introduction():
  ui.app_description(app_description)
  ui.report_verbose("verbose mode")
  ui.report_verbose("root path: " + base_path)
  ui.report_verbose("web page: " + webpage)  
  ui.report_verbose("retry wait: " + str(retry_wait) + "s")
  ui.report_verbose("debug_mode: " + str(debug_mode))
  ui.report_verbose()
  ui.report_info(ui.intro_entry("Number of LEDs", num_leds))
  print 

class Handler(BaseHTTPRequestHandler):
  global last_run, last_run_full, host_name, host_ip, to_run

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

  def banner(self):
    if last_run != '':
      return """
<div class="well well-sm clearfix">
  <div class="col-xs-9"><i class="fa fa-circle-o-notch fa-spin fa-fw"></i> %(last_run)s</div>
  <div class="col-xs-3"><a class="btn btn-link btn-xs pull-right" role="button" href="/command?run=stop"><span class="glyphicon glyphicon-remove-sign"></span></a></div>
</div>
      """ % globals()
    return ''

  def footer(self):
    return """
<div class="small text-center">
  %(host_name)s %(host_ip)s
</div>
<div class="small text-center">
  %(last_run_full)s
</div>
    """ % globals()

  def serve_page(self, page, headers={}):
    global last_run, last_run_full, host_name, host_ip
    filename, file_ext = os.path.splitext(page)
    filetime = datetime.datetime.fromtimestamp(os.path.getmtime(page))
    filetime_str = filetime.strftime("%a, %d %b %Y %H:%M:%S GMT")
    is_cached = False
    if headers != None:
      if 'If-Modified-Since' in headers:
        modified_since = headers['If-Modified-Since']
        if modified_since != None:
          print modified_since
          print filetime_str
          if modified_since.lower() == filetime_str.lower():
            is_cached = True
    if is_cached:
      self.send_response(304)
      self.send_header("Content-Length", "0")
      self.end_headers()
      return
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
    self.send_response(200)
    self.send_header("Content-type", content_type)
    # can't cache the html page until actions are backgrounded
    if content_type == "text/html":
      self.send_header("Cache-Control", "no-cache")
    else:
      self.send_header("Cache-Control", "private")
      self.send_header("Last-Modified", filetime_str)
    self.end_headers()  
    banner = self.banner()
    footer = self.footer()
    f = open(page, 'r')                       
    if content_type == "text/html":
    	self.wfile.write(f.read().replace('<!-- banner -->', banner).replace('<!-- footer -->', footer))
    else:
        self.wfile.write(f.read())
    f.close  
    self.wfile.close() 

  def handle_commands(self, commands):
    lc.command(":::")
    for cmd in commands:
      print cmd, 
      lc.command(cmd)

  def stop_running_app(self):
    global to_run
    if last_run != '':
      to_run = last_run_full
      self.kill_last_app();
    else:
      to_run = ''

  def restart_running_app(self):
    if to_run != '':
      self.run_app(to_run)

  def do_cmd(self, args):
    self.stop_running_app()
    self.handle_commands(["3:pau"] + args['cmd'] + [":3:cnt"])
    self.restart_running_app()

  def do_color(self, args):
    self.stop_running_app()
    self.handle_commands(["2:pau"] + args['color'] + [":1:cnt:flu"])
    self.restart_running_app()

  def do_macro(self, args):
    self.stop_running_app()
    self.handle_commands(["1:pau:2:cnt"] + [str(args['macro'][0]) + ":run"])
    self.restart_running_app()

  def do_run(self, args):
    self.kill_last_app()
    self.run_app(args['run'][0])

  def do_runonce(self, args):
    self.run_app(args['runonce'][0], False)

  def do_sys(self, args):
    for sys in args['sys']:
      self.log('shell command: ' + sys)
      call(sys, shell=True)

  def do_cast(self):
    pass

  def get_headers(self):
    return {'If-Modified-Since': self.headers.getheader('If-Modified-Since')}

  def do_webpage(self):
    headers = self.get_headers()
    # serve main page
    # self.serve_page(webpage, headers)
    # until command actions are handled asynchronously, need to
    # serve the page each time non-cached
    self.serve_page(webpage)

  def do_file(self, url):
    headers = self.get_headers()
    if os.path.isfile(base_path + url.path):
      page = base_path + url.path
      self.serve_page(page, headers)
      return True
    else:
      return False

  def do_notfound(self):
    msg = "<html><body><h1>404 Not Found</h1></body></html>"
    self.send_response(404)
    self.send_header("Content-Type", "text/html")
    self.send_header("Content-Length", len(msg))
    self.end_headers()
    self.wfile.write(msg)
    self.wfile.close()

  def do_command(self, url):
    args = urlparse.parse_qs(url.query)
    if 'cmd' in args:
      self.do_cmd(args)
    if 'color' in args:
      self.do_color(args)
    if 'macro' in args:
      self.do_macro(args)
    if 'run' in args:
      self.do_run(args)
    if 'runonce' in args:
      self.runonce(args)
    if 'sys' in args:
      self.do_sys(args)
    if 'cast' in args:
      self.do_cast(args)
    self.do_webpage()

  def do_GET(self):
    url = urlparse.urlparse(self.path)
    if url.path == '/command':
      self.do_command(url)
    else: 
      if not self.do_file(url):
        self.do_notfound()

############################################################################

def start_server():
  global httpd
  while(True):
    try:
      httpd = SocketServer.TCPServer(("", 8080), Handler)
    except socket_error,e:
      print "Error: " + str(e) + " - retrying"
      time.sleep(retry_wait)
      continue
    print "Listening..."
    break

def run_server():
  try:
    httpd.serve_forever()
  except KeyboardInterrupt:
    sys.exit("\nExiting...\n")

def setup():
  initialize()
  introduction()

def run():
  start_server()
  run_server()

def conclude():
  if last_run != '':
    print 'killing: ' + last_run
    call('killall ' + last_run, shell=True)

if __name__ == '__main__':
  setup()
  try:
    run() 
  except KeyboardInterrupt:
    sys.exit("\nExiting...\n")
  finally:
    conclude()


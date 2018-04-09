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
import struct
import threading
import fcntl

# ---------------------------------------------------------

global httpd, webpage, base_path, last_run, host_name, host_ip, app_description, verbose_mode, debug_mode, num_leds, macro_count, programs, macro_run_number, retry_wait, ip_address, port
global multicast_group_ip, multicast_port, timeout_in_seconds, multicast_group, num_times, no_keys, msg_delay

last_run = ''
last_run_full = ''
#host_name = socket.getfqdn(socket.gethostname())
#host_ip = socket.gethostbyname(socket.gethostname())
app_description = None
verbose_mode = None
debug_mode = None
num_leds = None
retry_wait = None
ip_address = None
port = None

def get_ip_address(ifname):
  s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
  return socket.inet_ntoa(fcntl.ioctl(
    s.fileno(),
    0x8915,  # SIOCGIFADDR
    struct.pack('256s', ifname[:15])
  )[20:24])

client_interface_name = 'apcli0'

def get_client_ip():
  return get_ip_address(client_interface_name)

def get_client_hostname():
  return socket.gethostname()

host_name = get_client_hostname()
host_ip = get_client_ip()

# ---------------------------------------------------------

def get_options():
  global verbose_mode, debug_mode, retry_wait, base_path, webpage, ip_address, port, multicast_group_ip, multicast_port, timeout_in_seconds, multicast_group, num_times, no_keys, msg_delay
  global multicast_group_ip, multicast_port, timeout_in_seconds, multicast_group, num_times, no_keys, msg_delay
  parser = argparse.ArgumentParser(description=app_description)
  parser.add_argument("webpage",  metavar="W",  nargs="?",                                                  help="path to web page")
  parser.add_argument("rootpath", metavar="R",  nargs="?",                                                  help="root path for files")
  parser.add_argument("-a",       "--addr",     dest="address",                      default="",            help='server address (all addresses)')
  parser.add_argument("-p",       "--port",     dest="port",    type=int,            default=8080,          help='server port (8080)')
  parser.add_argument("-r",       "--retry",    dest="retry",   type=int,            default=10,            help='retry wait (secs) (10)')
  parser.add_argument("-v",       "--verbose",  dest="verbose", action="store_true",                        help="display verbose info (False)")
  parser.add_argument("-d",       "--debug",    dest="debug",   action="store_true",                        help="display debugging info (False)")
  parser.add_argument("-m",       "--mcaddr",   dest="mcaddr",                       default='224.3.29.71', help='multicast group IP address (224.3.29.71)')
  parser.add_argument("-o",       "--mcport",   dest="mcport",  type=int,            default=10000,         help='multicast port (10000)')
  parser.add_argument("-t",       "--timeout",  dest="timeout", type=float,          default=0.1,           help='timeout time waiting for responses (seconds) (0.1)')
  parser.add_argument("-n",       "--numtimes", dest="times",   type=int,            default=15,            help='number of times to issue command (15)')
  parser.add_argument("-k",       "--nokeys",   dest="nokeys",  action='store_true',                        help='disables keys sent for dupe detection (False)')
  parser.add_argument("-e",       "--delay",    dest="delay",   type=float,          default=0.001,         help='delay exponent between duplicate messages (seconds) (0.001)')

  args = parser.parse_args()
  verbose_mode = args.verbose
  debug_mode = args.debug
  retry_wait = args.retry
  ip_address = args.address
  port = args.port
  base_path = args.rootpath
  if base_path == None:
    base_path = os.getcwd() + '/'
  webpage = args.webpage
  if webpage == None:
    webpage = base_path + 'http_command.html'
  multicast_group_ip = args.mcaddr
  multicast_port = args.mcport
  timeout_in_seconds = args.timeout
  multicast_group = (multicast_group_ip, multicast_port)
  num_times = args.times
  no_keys = args.nokeys
  msg_delay = args.delay

def validate_options():
    errors = False
    return not errors

# ---------------------------------------------------------

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
  ui.info_entry("client ip address", host_ip)
  ui.info_entry("server name", host_name)
  ui.info_entry("Number of LEDs", num_leds)
  print
#  ui.report_verbose()
  ui.report_verbose("verbose mode")
  ui.verbose_entry("root path", base_path)
  ui.verbose_entry("web page", webpage)  
  ui.verbose_entry("server ip_address", "all" if ip_address == '' else ip_address)
  ui.verbose_entry("port", str(port))
  ui.verbose_entry("multicast group IP", multicast_group_ip)
  ui.verbose_entry("multicast port", str(multicast_port))
  ui.verbose_entry("reply timeout", str(timeout_in_seconds) + "s")
  ui.verbose_entry("sends per message", str(num_times))
  ui.verbose_entry("sending keys", str(no_keys == False))
  ui.verbose_entry("message delay", str(msg_delay))
  ui.verbose_entry("retry wait", str(retry_wait) + "s")
  ui.verbose_entry("debug_mode", str(debug_mode))
  ui.report_verbose()
#  print 

# ---------------------------------------------------------

class Handler(BaseHTTPRequestHandler):
  global last_run, last_run_full, host_name, host_ip, to_rerun

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

  def content_type(self, file_path):
    filename, file_ext = os.path.splitext(file_path)
    content_type = "application/octet-stream"
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
    return content_type

  def is_cached(self, file_path, headers):
    filetime = datetime.datetime.fromtimestamp(os.path.getmtime(file_path))
    filetime_str = filetime.strftime("%a, %d %b %Y %H:%M:%S GMT")
    is_cached = False
    if headers != None:
      if 'If-Modified-Since' in headers:
        modified_since = headers['If-Modified-Since']
        if modified_since != None:
          #print modified_since
          #print filetime_str
          if modified_since.lower() == filetime_str.lower():
            is_cached = True
    return is_cached

  def serve_page(self, page, headers={}):
    global last_run, last_run_full, host_name, host_ip

#    filename, file_ext = os.path.splitext(page)
#

    filetime = datetime.datetime.fromtimestamp(os.path.getmtime(page))
    filetime_str = filetime.strftime("%a, %d %b %Y %H:%M:%S GMT")

#    is_cached = False
#    if headers != None:
#      if 'If-Modified-Since' in headers:
#        modified_since = headers['If-Modified-Since']
#        if modified_since != None:
#          #print modified_since
#          #print filetime_str
#          if modified_since.lower() == filetime_str.lower():
#            is_cached = True

    is_cached = self.is_cached(page, headers)
    if is_cached:
      self.send_response(304)
      self.send_header("Content-Length", "0")
      self.end_headers()
      return

    content_type = self.content_type(page)

#    if file_ext == '.html':
#      content_type = "text/html"                     
#    elif file_ext == '.css':
#      content_type = "text/css"                     
#    elif file_ext == '.js':
#      content_type = "application/javascript"
#    elif file_ext == '.ico':
#      content_type = "image/x-icon"
#    elif file_ext == '.png':
#      content_type = "image/png"
#    else:
#      content_type = "application/octet-stream"                     

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
    global to_rerun
    if last_run != '':
      to_rerun = last_run_full
      self.kill_last_app();
    else:
      to_rerun = ''

  def restart_running_app(self):
    if to_rerun != '':
      self.run_app(to_rerun)

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

  def do_cast(self, args):
    for cast in args['cast']:
      send_background_message(cast)

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

# ---------------------------------------------------------

def cast_socket():
  # Create the datagram socket
  sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

  # Set a timeout so the socket does not block indefinitely when trying
  # to receive data.
  sock.settimeout(timeout_in_seconds)

  # Set the time-to-live for messages to 1 so they do not go past the
  # local network segment.
  ttl = struct.pack('b', 1)
  sock.setsockopt(socket.IPPROTO_IP, socket.IP_MULTICAST_TTL, ttl)

  return sock

def add_key(command):
  return host_name + "/" + str(time.time()) + ";" + command

def send_socket_message(sock, message, times):
  if no_keys != True:
    message = add_key(message)
  for n in range(0, times):
    # Send data to the multicast group
    ui.report_verbose('sending "%s"' % message)
    sent = sock.sendto(message, multicast_group)
    if verbose_mode:
      while True:
        try:
          data, server = sock.recvfrom(256)
        #except KeyboardInterrupt:
        #  break
        except socket.timeout:
          break
        else:
          ui.report_verbose('received "%s" from %s' % (data, server))
    if n < (times - 1):
      time.sleep(msg_delay * (2 ** n))

def send_message(message):
  sock = cast_socket()
  send_socket_message(sock, message, num_times)
  sock.close()

background_threads = []

def handle_background_message(message):
  send_message(message)
  thread = threading.current_thread()
  background_threads.remove(thread)
  ui.report_verbose("terminating thread: " + str(thread))

def send_background_message(message):
  thread = threading.Thread(target=handle_background_message, args=(message,))
  ui.report_verbose("new thread: " + str(thread))
  background_threads.append(thread)
  thread.start()

def wait_for_active_threads():
  if(len(background_threads) > 0):
    ui.report_warn("waiting for active threads to terminate...")
    for t in background_threads:
      t.join()

############################################################################

def start_server():
  global httpd
  while(True):
    try:
      httpd = SocketServer.TCPServer((ip_address, port), Handler)
    except socket_error,e:
      ui.report_error("Error: " + str(e) + " - retrying")
      time.sleep(retry_wait)
      continue
    ui.report_info("Listening...")
    break

def run_server():
  try:
    httpd.serve_forever()
  except KeyboardInterrupt:
    ui.report_verbose("keyboard interupt")
    httpd.server_close()
    #sys.exit("\nExiting...\n")
    raise

def setup():
  initialize()
  introduction()

def run():
  start_server()
  run_server()

def conclude():
  if last_run != '':
    ui.report_info('killing: ' + last_run)
    call('killall ' + last_run, shell=True)

############################################################################
############################################################################

if __name__ == '__main__':
  setup()
  try:
    run() 
  except KeyboardInterrupt:
    pass
    sys.exit("\nExiting...\n")

  finally:
    wait_for_active_threads()
    conclude()
    sys.exit("\nExiting...\n")


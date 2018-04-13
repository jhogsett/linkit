#!/usr/bin/python

import socket
import struct
import sys
import terminal_colors as tc
import argparse
import time
import fcntl
import struct
import app_ui as ui
import threading

app_description = "LED Multicast Sender v.0.0 10-1-2017"

parser = argparse.ArgumentParser(description=app_description)
parser.add_argument("command",                      nargs='?',      default=None,          help='command to send & exit (optional)')
parser.add_argument("-v", "--verbose",              dest="verbose", action='store_true',   help='display verbose info (False)')
parser.add_argument("-i", "--ipaddr",               dest="ipaddr",  default='224.3.29.71', help='multicast group IP address (224.3.29.71)')
parser.add_argument("-p", "--port",     type=int,   dest="port",    default=10000,         help='multicast port (10000)')
parser.add_argument("-t", "--timeout",  type=float, dest="timeout", default=0.1,           help='timeout time waiting for responses (seconds) (0.1)')
parser.add_argument("-n", "--numtimes", type=int,   dest="times",   default=15,            help='number of times to issue command (9)')
parser.add_argument("-k", "--nokeys",               dest="nokeys",  action='store_true',   help='disables keys sent for dupe detection (False)')
parser.add_argument("-d", "--delay",    type=float, dest="delay",   default=0.001,         help='delay exponent between duplicate messages (seconds) (0.01)')

args = parser.parse_args()
command = args.command
verbose_mode = args.verbose
multicast_group_ip = args.ipaddr
multicast_port = args.port
timeout_in_seconds = args.timeout
multicast_group = (multicast_group_ip, multicast_port)
num_times = args.times
no_keys = args.nokeys
msg_delay = args.delay

ui.begin(verbose_mode)

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

# Create the datagram socket
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

# Set a timeout so the socket does not block indefinitely when trying
# to receive data.
sock.settimeout(timeout_in_seconds)

# Set the time-to-live for messages to 1 so they do not go past the
# local network segment.
ttl = struct.pack('b', 1)
sock.setsockopt(socket.IPPROTO_IP, socket.IP_MULTICAST_TTL, ttl)

#server_name = socket.getfqdn()
server_name = host_name

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

if command != None:
  send_background_message(command)
  sys.exit();

ui.app_description(app_description)
ui.report_verbose("verbose mode")
ui.info_entry("server name", server_name)
ui.info_entry("multicast group ip", multicast_group_ip)
ui.info_entry("multicast port", str(multicast_port))
ui.info_entry("reply timeout", str(timeout_in_seconds))
ui.info_entry("sends per message", str(num_times))
ui.info_entry("sending keys", str(no_keys == False))
ui.info_entry("message delay", str(msg_delay))
print

while True:
    try:
        cmd = raw_input(tc.yellow('command: '))
        if cmd == "":
            send_background_message(last_cmd)
            #send_message(last_cmd, num_times)
        else:
           send_background_message(cmd)
           #send_message(cmd, num_times)
           last_cmd = cmd
    except EOFError:
        sock.close()
        sys.exit("\nExiting...\n")
        wait_for_active_threads()
    except KeyboardInterrupt:
        sock.close()
        wait_for_active_threads()
        sys.exit("\nExiting...\n")
    except Exception:
        sock.close()
        raise


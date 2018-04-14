#!/usr/bin/python

import socket
import struct
import sys
import serial
import time
import select
import terminal_colors as tc
import argparse
import led_command as lc
import app_ui as ui
import fcntl

app_description = "LED Multicast Commander v.0.0 10-1-2017"
timeout_in_seconds = 10
multicast_group = '224.3.29.71'
server_port = 10000
response_wait = 0.1
verbose_mode = False
response_wait = 0.1

parser = argparse.ArgumentParser(description=app_description)
parser.add_argument("-v", "--verbose",           dest="verbose", action='store_true',   help='display verbose info (False)')
parser.add_argument("-i", "--ipaddr",            dest="ipaddr",  default='224.3.29.71', help='multicast group IP address (224.3.29.71)')
parser.add_argument("-p", "--port",    type=int, dest="port",    default=10000,         help='multicast port (10000)')
parser.add_argument("-t", "--timeout", type=int, dest="timeout", default=10,            help='receiving period in seconds (10)')

args = parser.parse_args()
verbose_mode = args.verbose
multicast_group_ip = args.ipaddr
server_port = args.port
timeout_in_seconds = args.timeout
server_address = ('', server_port)

lc.begin(verbose_mode)
ui.begin(verbose_mode)
#lc.attention()
#lc.stop_all()
#lc.command("cnt")

# Create the socket
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

global keylist 
keylist = []
numkeys = 20

def get_key(message):
  return message.split(";")[0]

def get_command(message):
  return message.split(";")[1]

def handle_command(cmd_text):
    """
    returns false if this was a duplicate command
    """
    global keylist

    cmd = ""
    key = ""
    if ";" in cmd_text:
        #key, cmd = cmd_text.split(";")
        key = get_key(cmd_text)
        cmd = get_command(cmd_text)

        if key in keylist:
            ui.report_verbose('skipping duplicate command')
            return False

        keylist.append(key)
        keylist = keylist[-numkeys:]
    else:
        # no key, always issue command
        key = "no key"
        cmd = cmd_text
        watermark = 0

    if verbose_mode:
        ui.verbose_entry("command", cmd)
        ui.verbose_entry("key", key)
    else:
        ui.info_entry("command", cmd)

    lc.command(":::")
    lc.command(cmd)
    lc.flush_output();
    return True

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

multicast_group = multicast_group_ip
server_address = ('', server_port)

ui.app_description(app_description)
ui.report_verbose("verbose mode")
ui.info_entry("host name", host_name)
ui.info_entry("host ip", host_ip)
ui.info_entry("multicast group IP", multicast_group_ip)
ui.info_entry("server port", str(server_port))
ui.info_entry("receiving period", str(timeout_in_seconds) + "s")
print

# key looks like   return host_name + "/" + str(time.time()) + ";" + command
def create_ack(key):
  return host_name + "/" + str(time.time()) + ";ack;" + key

# Receive/respond loop
while True:
    # Create the socket
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

    # Bind to the server address
    sock.bind(server_address)

    # Tell the operating system to add the socket to the multicast group on all interfaces.
    group = socket.inet_aton(multicast_group)
    mreq = struct.pack('4sL', group, socket.INADDR_ANY)
    sock.setsockopt(socket.IPPROTO_IP, socket.IP_ADD_MEMBERSHIP, mreq)

    sock.setblocking(0)

    try:
        ui.report_verbose('\nwaiting ' + str(timeout_in_seconds) + 's to receive message...')
        ready = select.select([sock], [], [], timeout_in_seconds)
        if ready[0]:
            data, address = sock.recvfrom(1024)
            ui.report_verbose('received %s bytes from %s' % (len(data), address))            

            if handle_command(data):
                ack = create_ack(get_key(data))
                ui.report_verbose('sending acknowledgement to ' + str(address) + ': ' + ack)
                sock.sendto(ack, address)
        sock.close()
    except EOFError:
        sys.exit("\ngoodbye.\n")
    except KeyboardInterrupt:
        sys.exit("\ngoodbye.\n")
    except Exception:
        raise


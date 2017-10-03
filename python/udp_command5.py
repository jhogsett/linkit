#!/usr/bin/python

import socket
import struct
import sys
import serial
import time
import select
import terminal_colors as tc
import argparse

app_description = "LED Multicast Commander v.0.0 10-1-2017"
timeout_in_seconds = 10
multicast_group = '224.3.29.71'
server_port = 10000
response_wait = 0.1
verbose_mode = False

global s
s = serial.Serial("/dev/ttyS0", 115200)

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

# Create the socket
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

def flush_input():
    s.flushInput()

def flush_output():
    s.flushOutput()

def wait_for_ack():
    time.sleep(response_wait)
    while s.inWaiting() <= 0:
        pass
    time.sleep(response_wait)
    while s.inWaiting() > 0:
        received = s.read(s.inWaiting())
        if verbose_mode:
            print received

    if verbose_mode:
        print

def command(cmd_text):
    s.write((cmd_text + ':').encode())
    wait_for_ack()

global keylist 
keylist = []
numkeys = 20

def handle_command(cmd_text):
    global keylist

    cmd = ""
    key = ""
    if ";" in cmd_text:
        key, cmd = cmd_text.split(";")

        if key in keylist:
            if verbose_mode:
                print >>sys.stderr, tc.red('skipping duplicate command')
            return

        keylist.append(key)
        keylist = keylist[-numkeys:]
    else:
        # no key, always issue command
        key = "no key"
        cmd = cmd_text
        watermark = 0

    if verbose_mode:
        print >>sys.stderr, tc.yellow("command: ") + tc.white(cmd) + tc.red(" (key: " + key + ")")
    else:
        print >>sys.stderr, tc.yellow("command: ") + tc.white(cmd)
    command(":::3:pau")
    command(":::3:cnt:" + cmd)
    flush_output();

multicast_group = multicast_group_ip
server_address = ('', server_port)
client_name = socket.getfqdn()

print tc.magenta("\n" + app_description + "\n")
if verbose_mode:
    print tc.yellow("verbose mode")
print tc.cyan("client name: ") + tc.green(client_name)
print tc.cyan("multicast group IP: ") + tc.green(multicast_group_ip)
print tc.cyan("server port: ") + tc.green(str(server_port))
print tc.cyan("receiving period: ") + tc.green(str(timeout_in_seconds) + "s")
print

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
        if verbose_mode:
            print >>sys.stderr, '\nwaiting ' + str(timeout_in_seconds) + 's to receive message...'
        ready = select.select([sock], [], [], timeout_in_seconds)
        if ready[0]:
            data, address = sock.recvfrom(1024)
    
            if verbose_mode:
            	print >>sys.stderr, tc.green('received %s bytes from %s' % (len(data), address))

            handle_command(data)

            if verbose_mode:
                print >>sys.stderr, tc.cyan('sending acknowledgement to ' + str(address))
            sock.sendto('ack', address)

        sock.close()
    except EOFError:
        sys.exit("\ngoodbye.\n")
    except KeyboardInterrupt:
        sys.exit("\ngoodbye.\n")
    except Exception:
        raise


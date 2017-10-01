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
multicast_group = args.ipaddr
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
        print '-',   
 
    time.sleep(response_wait)
    while s.inWaiting() > 0:
        print s.read(s.inWaiting()),
    print

def command(cmd_text):
    s.write((cmd_text + ':').encode())
    wait_for_ack()

global watermark
watermark = 0

def handle_command(cmd_text):
    global watermark
    new_watermark = 0

    cmd = ""
    if ";" in cmd_text:
        sequence, cmd = cmd_text.split(";")
        try:
            new_watermark = int(sequence)
            if new_watermark > 0 and new_watermark <= watermark:
                print >>sys.stderr, 'skipping duplicate command'
                return    
            watermark = new_watermark
        except ValueError:
            # if not a number, reset watermark and issue command
            watermark = 0
    else:
       # no sequence number, issue command and reset watermark
        cmd = cmd_text
        watermark = 0
    print >>sys.stderr, 'issuing command...'

    command(":::" + cmd)
    flush_output();

multicast_group = '224.3.29.71'
server_address = ('', 10000)

# Create the socket
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

# Bind to the server address
sock.bind(server_address)

# Tell the operating system to add the socket to the multicast group on all interfaces.
group = socket.inet_aton(multicast_group)
mreq = struct.pack('4sL', group, socket.INADDR_ANY)
sock.setsockopt(socket.IPPROTO_IP, socket.IP_ADD_MEMBERSHIP, mreq)

sock.setblocking(0)

# Receive/respond loop
while True:

    try:
        print >>sys.stderr, '\nwaiting 10s to receive message...'
        ready = select.select([sock], [], [], timeout_in_seconds)
        if ready[0]:
            data, address = sock.recvfrom(1024)
    
            print >>sys.stderr, 'received %s bytes from %s' % (len(data), address)
            print >>sys.stderr, data

            handle_command(data)

            print >>sys.stderr, 'sending acknowledgement to', address
            sock.sendto('ack', address)

    except EOFError:
        sys.exit("\ngoodbye.\n")
    except KeyboardInterrupt:
        sys.exit("\ngoodbye.\n")
    except Exception:
        raise


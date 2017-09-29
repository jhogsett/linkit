#!/usr/bin/python

import socket
import struct
import sys
import serial
import time

multicast_group = '224.3.29.71'
server_address = ('', 10000)

global s
s = serial.Serial("/dev/ttyS0", 115200)

response_wait = 0.1

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
    print s.read(s.inWaiting()),
  print

def command(cmd_text):
  s.write((cmd_text + ':').encode())
  wait_for_ack()

def handle_command(cmd_text):
  #command(":::pau:pau")
  command(":::")
  command(cmd_text)
  #command("cnt")

# Bind to the server address
sock.bind(server_address)

# Tell the operating system to add the socket to the multicast group on all interfaces.
group = socket.inet_aton(multicast_group)
mreq = struct.pack('4sL', group, socket.INADDR_ANY)
sock.setsockopt(socket.IPPROTO_IP, socket.IP_ADD_MEMBERSHIP, mreq)

# Receive/respond loop
while True:
    print >>sys.stderr, '\nwaiting to receive message'
    data, address = sock.recvfrom(1024)
    
    print >>sys.stderr, 'received %s bytes from %s' % (len(data), address)
    print >>sys.stderr, data

    print >>sys.stderr, 'issuing command...'
    handle_command(data)

#    print >>sys.stderr, 'sending acknowledgement to', address
#    sock.sendto('ack', address)



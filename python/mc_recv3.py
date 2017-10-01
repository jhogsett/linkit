#!/usr/bin/python

import socket
import struct
import sys
import time
import select

timeout_in_seconds = 10
multicast_group = '224.3.29.71'
server_address = ('', 10000)

# Create the socket
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

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

  print >>sys.stderr, 'received command ' + cmd

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

    print >>sys.stderr, '\nwaiting 10s to receive message...'
    ready = select.select([sock], [], [], timeout_in_seconds)
    if ready[0]:
        data, address = sock.recvfrom(1024)
    
        print >>sys.stderr, 'received %s bytes from %s' % (len(data), address)
        print >>sys.stderr, data

        handle_command(data)

        print >>sys.stderr, 'sending acknowledgement to', address
        sock.sendto('ack', address)


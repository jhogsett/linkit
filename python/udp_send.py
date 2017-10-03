#!/usr/bin/python

import socket
import struct
import sys
import terminal_colors as tc
import argparse
import time

app_description = "LED Multicast Sender v.0.0 10-1-2017"
timeout_in_seconds = 0.5
multicast_group_ip = '224.3.29.71'
multicast_port = 10000
response_wait = 0.1
verbose_mode = False

parser = argparse.ArgumentParser(description=app_description)
parser.add_argument("command",                      nargs='?',      default=None,          help='command to send & exit (optional)')
parser.add_argument("-v", "--verbose",              dest="verbose", action='store_true',   help='display verbose info (False)')
parser.add_argument("-i", "--ipaddr",               dest="ipaddr",  default='224.3.29.71', help='multicast group IP address (224.3.29.71)')
parser.add_argument("-p", "--port",     type=int,   dest="port",    default=10000,         help='multicast port (10000)')
parser.add_argument("-t", "--timeout",  type=float, dest="timeout", default=0.5,           help='socket in seconds (0.5)')
parser.add_argument("-n", "--numtimes", type=int,   dest="times",   default=1,             help='number of times to issue command (1)')
parser.add_argument("-k", "--nokeys",               dest="nokeys",  action='store_true',   help='disables keys sent for dupe detection (False)')

args = parser.parse_args()
command = args.command
verbose_mode = args.verbose
multicast_group_ip = args.ipaddr
multicast_port = args.port
timeout_in_seconds = args.timeout
multicast_group = (multicast_group_ip, multicast_port)
num_times = args.times
no_keys = args.nokeys

# Create the datagram socket
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

# Set a timeout so the socket does not block indefinitely when trying
# to receive data.
sock.settimeout(timeout_in_seconds)

# Set the time-to-live for messages to 1 so they do not go past the
# local network segment.
ttl = struct.pack('b', 1)
sock.setsockopt(socket.IPPROTO_IP, socket.IP_MULTICAST_TTL, ttl)

server_name = socket.getfqdn()

print tc.magenta("\n" + app_description + "\n")
if verbose_mode:
    print tc.yellow("verbose mode")
print tc.cyan("server name: ") + tc.green(server_name)
print tc.cyan("multicast group IP: ") + tc.green(multicast_group_ip)
print tc.cyan("multicast port: ") + tc.green(str(multicast_port))
print tc.cyan("reply timeout: ") + tc.green(str(timeout_in_seconds) + "s")
print tc.cyan("sends per message: ") + tc.green(str(num_times))
print tc.cyan("sending keys: ") + tc.green(str(no_keys == False))
print

def add_key(command):
    return server_name + "/" + str(time.time()) + ";" + command

def send_message(message, times):
    if no_keys != True:
        message = add_key(message)
    for n in range(0, times):
        # Send data to the multicast group
	if verbose_mode:        
            print >>sys.stderr, tc.green('sending "%s"' % message)
        sent = sock.sendto(message, multicast_group)

    # Look for responses from all recipients
    while True:
        try:
            data, server = sock.recvfrom(16)
        except socket.timeout:
            #print >>sys.stderr, 'timed out, no more responses'
            break
        else:
            if verbose_mode:
                print >>sys.stderr, tc.red('received "%s" from %s' % (data, server))

if command != None:
    send_message(command, num_times)
    sock.close()
    exit(0);

cmd = ""
last_cmd = cmd

while True:
    try:
        cmd = raw_input(tc.yellow('command: '))
        if cmd == "":
            send_message(last_cmd, num_times)
        else:
           send_message(cmd, num_times)
           last_cmd = cmd

    except EOFError:
        sock.close()
        sys.exit("\nExiting...\n")
    except KeyboardInterrupt:
        sock.close()
        sys.exit("\nExiting...\n")
    except Exception:
        sock.close()
        raise


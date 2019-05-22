import socket
from socket import error as socket_error
import struct
import threading
import time

import app_ui as ui

global host_name, verbose_mode, multicast_group_ip, multicast_port, timeout_in_seconds, multicast_group, num_times, no_keys, msg_delay
global responses, background_threads
responses = {}
background_threads = []

def begin(host_name_, verbose_mode_=False, multicast_group_ip_=None, multicast_port_=None, timeout_in_seconds_=None, num_times_=None, msg_delay_=None):
    global host_name, verbose_mode, multicast_group_ip, multicast_port, timeout_in_seconds, multicast_group, num_times, no_keys, msg_delay

    host_name = host_name_
    verbose_mode = verbose_mode_
    multicast_group_ip = multicast_group_ip_ or '224.3.29.71'
    multicast_port = multicast_port_         or 10000
    timeout_in_seconds = timeout_in_seconds_ or 0.1
    num_times = num_times_                   or 15
    no_keys = False
    msg_delay = msg_delay_                   or 0.001

    multicast_group = (multicast_group_ip, multicast_port)

    ui.begin(verbose_mode)

def conclude():
    wait_for_active_threads()

def broadcast(message, targets=None):
    message = create_key(message, targets)
    send_background_message(message)

def received():
    return responses

# ========================================

def create_key(message, targets):
    key = host_name + "/" + str(time.time())
    if targets:
        key = key + "/" + targets
    key = key + ";" + message
    return key

# ========================================

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

def send_socket_message(sock, message, times):
    global responses
    responses = {}

    for n in range(0, times):
        ui.report_verbose('sending "%s"' % message)
        sent = sock.sendto(message, multicast_group)

        while True:
            try:
                data, server = sock.recvfrom(256)
            except socket.timeout:
                break
            ip = server[0].strip("'")
            responses[ip] = data
            ui.report_verbose('received "%s" from %s' % (data, ip))

        if n < (times - 1):
            time.sleep(msg_delay * (2 ** n))

def send_message(message):
    sock = cast_socket()
    send_socket_message(sock, message, num_times)
    sock.close()

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
############################################################################

#if __name__ == '__main__':
#  setup()
#  try:
#    run()
#  except KeyboardInterrupt:
#    pass
#    sys.exit("\nExiting...\n")
#
#  finally:
#    wait_for_active_threads()
#    conclude()
#    sys.exit("\nExiting...\n")


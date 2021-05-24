import time
import psutil

def network():
    old_value = 0    
    global net;
    while True:
        new_value = psutil.net_io_counters().bytes_sent + psutil.net_io_counters().bytes_recv

        if old_value:
            net=send_stat(new_value - old_value)
            return net

        old_value = new_value

        time.sleep(1)

def convert_to_gbit(value):
    return value/1024./1024./1024.*8

def send_stat(value):
    return "%0.6f" % convert_to_gbit(value)

while True:
	print(network())
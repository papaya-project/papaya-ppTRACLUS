"""
 File client.cpp
 Authors: Zied Becha (becha@eurecom.fr) 
 Description: This script is used to transfer the shares with the second server in two non-colluding servers mode
 Usage: python3 client.py IPADRESS FILENAME
"""

import socket
import sys
files = ["example.csv","threshold.csv","minLns.csv","cluster.csv"]
s = socket.socket()
s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
s.connect((sys.argv[1],int(sys.argv[2])))
with open (files[int(sys.argv[3])], "rb") as infile:
	l = infile.read(1024)
	while (l):
		s.send(l)
		l = infile.read(1024)
s.close()


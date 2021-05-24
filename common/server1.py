
#USAGE
#python3 server.py IPADRESS FILENAME
import socket
import sys
s = socket.socket()
s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
s.bind((sys.argv[1],int(sys.argv[2])))
s.listen(10)
sc, address = s.accept()	
 # Accepts up to 10 connections.
files = ["example.csv","threshold.csv","minLns.csv","cluster.csv"]
with open (files[int(sys.argv[3])], "rb") as infile:
	l = infile.read(1024)
	while (l):
		sc.send(l)
		l = infile.read(1024)


infile.close()

sc.close()
s.close()

#USAGE
#python3 server.py IPADRESS FILENAME
import socket
import sys
s = socket.socket()
s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
s.bind((sys.argv[1],9999))
s.listen(10) # Accepts up to 10 connections.
files = ["example.csv","threshold.csv","minLns.csv","cluster.csv"]


sc, address = s.accept()
print (address)
with open(files[int(sys.argv[2])],'wb') as f: 
# receive data and write it to file
	l = sc.recv(1024)
	while (l):
		f.write(l)
		l = sc.recv(1024)
f.close()

sc.close()

s.close()
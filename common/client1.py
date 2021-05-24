
#USAGE
#python3 client.py IPADRESS FILENAME
import socket
import sys
files = ["example.csv","threshold.csv","minLns.csv","cluster.csv"]
s = socket.socket()
s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
s.connect((sys.argv[1],int(sys.argv[2])))

	
with open(files[int(sys.argv[3])],'wb') as f: 
# receive data and write it to file
	l = s.recv(1024)
	while (l):
		f.write(l)
		l = s.recv(1024)

s.close()


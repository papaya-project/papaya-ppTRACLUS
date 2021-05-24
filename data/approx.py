import sys
def time(x):
	return 8.54*x+13

def data(x):
	return 889079*x+14476
print(time(400))
t=0
d=0
for i in range(int(sys.argv[1])):
	t+=time(i+1)
	d+=data(i+1)
print(str((t/1000)/60)+" minutes")
print(str(d)+" bytes")
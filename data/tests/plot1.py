import matplotlib.pyplot as plt
time_argentera = []
time_argenterav4 = []
time_ibm = []
time_ibmargentera = []
time_lombarde = []
argenterav4=[50,100,150,200,250,300,350,400,450,500,550,600,650,700,750,800,850,900]
argentera=[50,100,150,200,250,300,350,400,450,500,550,600,650,700,750,800,850,900]

file=open("argentera/time.txt","r")
lines = file.readlines()
for line in lines:
	l=line.split(":")
	t = int(l[0])*3600+int(l[1])*60+int(l[2])
	time_argentera.append(t)

file=open("argentera/timev4.txt","r")
lines = file.readlines()
for line in lines:
	l=line.split(":")
	t = int(l[0])*3600+int(l[1])*60+int(l[2])
	time_argenterav4.append(t)


plt.figure()
plt.ylabel('Time(s)')
plt.xlabel('Line Segments')
plt.title('Time taken per line segments')
plt.plot(argentera,time_argentera,label="Argentera v3")
plt.plot(argenterav4,time_argenterav4,label="Argentera v4")
plt.grid()
plt.legend(loc='upper left', frameon=False)
plt.savefig("time.png")
import matplotlib.pyplot as plt
time_argentera = []
time_ibm = []
time_ibmargentera = []
time_lombarde = []
#argenterav4=[50,100,150,200,250,300,350,400,450,500,550,600,650,700,750,800,850,900,950,1000,1050,1100,1150,1200,1250]
argentera=[50,100,150,200,250,300,350,400,450,500,550,600,650,700,750,800,850,900]

file=open("time.txt","r")
lines = file.readlines()
for line in lines:
	l=line.split(":")
	t = int(l[0])*3600+int(l[1])*60+int(l[2])
	time_argentera.append(t)



plt.figure()
plt.ylabel('Time(s)')
plt.xlabel('Line Segments')
plt.title('Time taken per line segments')
plt.plot(argentera,time_argentera,label="Argentera")

plt.grid()
#plt.legend(loc='upper right', frameon=False)
plt.savefig("time.png")
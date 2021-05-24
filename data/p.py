import matplotlib.pyplot as plt
time = []
data= []
with open("logs.txt") as file:
	lines = file.readlines()
	for line in lines:
		if "Total =	" in line:
			time.append(float(line.split("=		")[1].split(" ")[0]))
		if "Total Sent / Rcv	" in line:
			data.append(float(line.split("\t")[1].split(" ")[0]))

print(time[500])
print(time[200])
plt.figure()
plt.ylabel('Time')
plt.xlabel('Iteration')
plt.title('Time taken per iteration')
plt.plot(time)
plt.savefig("time.png")

print(data[500])
print(data[200])
plt.figure()
plt.ylabel('Data (bytes)')
plt.xlabel('Iteration')
plt.title('Data exchanged per iteration')
plt.plot(data)
plt.savefig("data.png")
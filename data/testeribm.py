import subprocess
import os
import time

for i in range(10,3000,10):
	time.sleep(2)
	print("[-] Testing for "+str(i)+" Line Segments")
	before = FreeMemory()
	process = subprocess.Popen("time curl -F 'file=@example_route_cleaned.csv' http://0.0.0.0:5002/start/"+str(i)+"/3/3800/3/1/x/2 > tests/"+str(i)+"-v3.txt 2>&1", shell=True, stdout=subprocess.PIPE)
	after=100000000
	while process.poll() is None:
		after1=FreeMemory()
		time.sleep(1)
		if(after>after1):
			after = after1
	print("[-] Output Saved Under tests/"+str(i)+"-v3.txt")
	print ('[*] Memory used: %2.3f GB' % (before - after))
	f.write('[*] Line segments : '+str(i)+' Memory used: %2.3f GB\n' % (before - after))
"""
 File app.py
 Authors: Zied Becha (becha@eurecom.fr) and Orhan Ermis (ermis@eurecom.fr)
 Description: Implementation of the APIs for client-server and two non-colluding servers modes of Privacy-preserving Trajectory Clustering
"""

from flask import Flask, escape, request,jsonify, send_from_directory
import subprocess
import os
import requests
import subprocess
from werkzeug.utils import secure_filename
import time

app = Flask(__name__)

HOST = '0.0.0.0'
HTTP_PORT = 5555
TCP_PORT = 9999

UPLOAD_DIRECTORY = '/app/config_files'
app = Flask(__name__)




def init():
	global IP_PUB;
	global PORT_PUB;
	global URL_PUB;
	global IP_PUB_2;
	global PORT_PUB_2;
	global URL_PUB_2;
	global IBM;
	global LENGTH;
	with open("infos.txt","r") as f:
		line = f.readlines()
		LENGTH=len(line)
		IBM=str(line[0]).strip()
		IP_PUB=str(line[1]).strip()
		PORT_PUB=str(line[2]).strip()
		URL_PUB=str(line[3]).strip()
		try:
			IP_PUB_2=str(line[4]).strip()
			PORT_PUB_2=str(line[5]).strip()
			URL_PUB_2=str(line[6]).strip()
		except:
			pass

def check():
	if os.path.exists("infos.txt"):
		print('[-] Server(s) information has been initialized successfully!')
		return True
	else:
		print('[-] ERROR: Please check the server information and try again!')
		return False

def mode0(e,minlns,threshold,itera):
	print("[-] Starting server")
	try:
		r = requests.post('http'+IBM+'://'+URL_PUB+'/start/'+e,timeout=0.5)
		
	except requests.exceptions.ReadTimeout: 
		pass

	print("[-] Starting Client")
	os.system('./client -w 0 -e '+str(e)+' -a '+ str(IP_PUB)+' -p '+str(PORT_PUB)+' -t '+threshold+' -m '+minlns+' -i '+FILE_PUB)
	print("[-] Exchanging data")
	for filenumber in range(3):
		try:
			requests.post('http'+IBM+'://'+URL_PUB_2+'/receive/'+str(filenumber),timeout=0.5)
		except requests.exceptions.ReadTimeout: 
			pass	
		requests.post('http://127.0.0.1:5555/send/'+str(filenumber))
		

	
	path2 = "example.csv"
	print("[-] Starting Clustering")
	try:
		requests.post('http'+IBM+'://'+URL_PUB+'/round2/'+e+'/'+path2+'/'+itera,timeout=0.5)
	except requests.exceptions.ReadTimeout: 
		pass

	requests.post('http'+IBM+'://'+URL_PUB_2+'/round2/'+e+'/'+path2+'/'+itera+'/'+IP_PUB+'/'+PORT_PUB)
	
	
	
	
	while True:
		time.sleep(5)
		re = requests.post('http'+IBM+'://'+URL_PUB+'/check/'+e)
		if "done" in re.text:
			break;

	while True:
		time.sleep(5)
		re = requests.post('http'+IBM+'://'+URL_PUB_2+'/check/'+e)
		if "done" in re.text:
			break;
		
	
	
	print("[-] Sending Data To Client")
	try:
		requests.post('http'+IBM+'://'+URL_PUB_2+'/send/3',timeout=0.5)
		
	except requests.exceptions.ReadTimeout: 
		pass
	requests.post('http://127.0.0.1:5555/receive/3')
	time.sleep(1)

	path3='cluster.csv'
	print("[-] Decrypt Data")
	try:
		requests.post('http'+IBM+'://'+URL_PUB+'/round3/'+e+'/'+path3,timeout=0.5)
	except requests.exceptions.ReadTimeout: 
		pass
	r = requests.post('http://127.0.0.1:5555/round3/'+e+'/'+path3)
	out= (r.text).split('\n')
	for i in out:
		print(i)

	results=[]

	

	with open("data/cluster_un.csv" ,'r') as file:
		for line in file:
				results.append(line[0]+' ,')
	return results



def mode1(e,minlns,threshold,itera):
	print(URL_PUB)

	try:
		print('http'+IBM+'://'+URL_PUB+'/start/'+e)
		requests.post('http'+IBM+'://'+URL_PUB+'/start/'+e,timeout=0.5)
	except requests.exceptions.ReadTimeout: 
		pass
	print("[-] Starting server")
#	os.system('nohup curl http://'+URL_PUB+'/start/'+e) # IBM

	print("[-] Starting Client")
	os.system('./client -w 0 -e '+str(e)+' -a '+ str(IP_PUB)+' -p '+str(PORT_PUB)+' -t '+threshold+' -m '+minlns+' -i '+FILE_PUB)
	print("[-] Shares Created")
	path2 = "example.csv"

	try:
		requests.post('http'+IBM+'://'+URL_PUB+'/round2/'+e+'/'+path2+'/'+itera,timeout=0.5)
	except requests.exceptions.ReadTimeout: 
		pass
	print("[-] Starting Custering")
#	os.system('nohup curl http://'+URL_PUB+'/round2/'+e+'/'+path2+'/'+itera) # IBM
	r = requests.post('http://127.0.0.1:5555/round2/'+e+'/'+path2+'/'+itera) 
	if ("Process Ended" in r.text):
		return 'Process Killed'
#	os.system('curl http://127.0.0.1:5555/round2/'+e+'/'+path2+'/'+itera) # IBM

	while True:
		time.sleep(5)
		re = requests.post('http'+IBM+'://'+URL_PUB+'/check/'+e)
		if "done" in re.text:
			break;

	while True:
		time.sleep(5)
		re = requests.post('http://127.0.0.1:5555/check/'+e)
		if "done" in re.text:
			break;

	print("[-] Custering Completed")
	path3='cluster.csv'
	try:
		requests.post('http'+IBM+'://'+URL_PUB+'/round3/'+e+'/'+path3,timeout=0.5)
	except requests.exceptions.ReadTimeout: 
		pass
	print("[-] Getting Results")
#	os.system('nohup curl http://'+URL_PUB+'/round3/'+e+'/'+path3) # IBM

	r = requests.post('http://127.0.0.1:5555/round3/'+e+'/'+path3)
#	process = subprocess.Popen('curl http://127.0.0.1:5555/round3/'+e+'/'+path3, shell=True, stdout=subprocess.PIPE)
#	out, err = process.communicate()
	out= (r.text).split('\n')
	for i in out:
		print(i)
	results=[]
	with open("data/cluster_un.csv" ,'r') as file:
		for line in file:
				results.append(line[0]+' ,')
	print("[-] Results Sent to Client")
	return results


@app.route('/init0/<ipaddress>/<port>/<url>/<ipaddress1>/<port1>/<url1>', methods=['GET', 'POST'])
def init0(ipaddress,port,url,url1,ipaddress1,port1):
	with open("infos.txt","w") as f:
		if "papaya" in str(url):
			f.write('s\n')
		else:
			f.write('\n')
		f.write(str(ipaddress)+'\n')
		f.write(str(port)+'\n')
		f.write(str(url)+'\n')
		f.write(str(ipaddress1)+'\n')
		f.write(str(port1)+'\n')
		f.write(str(url1)+'\n')
	return jsonify({'Message': 'Server information have been updated successfully'})

@app.route('/init1/<ipaddress>/<port>/<url>', methods=['GET', 'POST'])
def init1(ipaddress,port,url):
	with open("infos.txt","w") as f:
		if "papaya" in str(url):
			f.write('s\n')
		else:
			f.write('\n')
		f.write(str(ipaddress)+'\n')
		f.write(str(port)+'\n')
		f.write(str(url)+'\n')
	return jsonify({'Message': 'Server information have been updated successfully'})



#@app.route('/start/<ipaddress>/<ipaddress2>/<ipaddress3>/<e>/<minlns>/<threshold>/<path>/<itera>/<mode>', methods=['GET', 'POST'])
#def startclient(e,ipaddress,minlns,threshold,path,ipaddress3,ipaddress2,mode,itera):
@app.route('/start/<e>/<minlns>/<threshold>/<itera>', methods=['GET', 'POST'])
def startclient(e,minlns,threshold,itera):		
	if not check():
		return jsonify({'Error': 'Please check the server information and try again!'})
	upload_file()

	results= None
	init()
	print("[-] Cleaning Up")
	reset()
	requests.post('http'+IBM+'://'+URL_PUB+'/remove')
	try:
		requests.post('http'+IBM+'://'+URL_PUB_2+'/remove')
	except:
		pass
	remove()

	if (not os.path.isfile(FILE_PUB)):
		return jsonify({'message': 'File does not exist'})
	if LENGTH>4:
		results = mode0(e,minlns,threshold,itera)
	else:
		results = mode1(e,minlns,threshold,itera)
	if ("Process Killed" in results):
		return jsonify({'message': 'Process Killed'})
	results=clean_cluster(results)
	return jsonify({'Clusters': "".join(results)[0:-2]})

def clean_cluster(results):
	for i in range(len(results)):
		if (results.count(results[i])==1):
			results[i]='0 ,'
	return results

@app.route('/receive/<filenumber>', methods=['GET', 'POST'])
def receivefiles(filenumber):
	print ('python3 client1.py '+IP_PUB_2+' '+PORT_PUB_2+' '+filenumber)
	os.system('python3 client1.py '+IP_PUB_2+' '+PORT_PUB_2+' '+filenumber)
	return jsonify({'message': 'receive started'})


@app.route('/send/<filenumber>', methods=['GET', 'POST'])
def sendfiles(filenumber):
	print ('python3 client.py '+IP_PUB_2+' '+PORT_PUB_2+' '+ filenumber)
	os.system('python3 client.py '+IP_PUB_2+' '+PORT_PUB_2+' '+ filenumber)
	return jsonify({'message': 'Sending started'})

@app.route('/round2/<e>/<path>/<itera>', methods=['GET', 'POST'])
def round2(e,path,itera):
	print ('./client -w 1 -e '+str(e)+' -a '+ str(IP_PUB)+' -p '+str(PORT_PUB)+' -i '+path+' -l '+itera)
	os.system('./client -w 1 -e '+str(e)+' -a '+ str(IP_PUB)+' -p '+str(PORT_PUB)+' -i '+path+' -l '+itera)
	return jsonify({'message': 'Round2 started'})

@app.route('/round3/<e>/<path>', methods=['GET', 'POST'])
def round3(e,path):
	print ('./client -w 2 -e '+str(e)+' -a '+ str(IP_PUB)+' -p '+str(PORT_PUB)+' -i '+path)
	os.system('./client -w 2 -e '+str(e)+' -a '+ str(IP_PUB)+' -p '+str(PORT_PUB)+' -i '+path)
	return jsonify({'message': 'Round 3 finished'})


@app.route('/uploader', methods = ['GET', 'POST'])
def upload_file():
	if request.method == 'POST':
		f = request.files['file']
		fname = secure_filename(f.filename);
      
		check = str(f.read(), 'utf-8')
		check1 = check.split('\n')
		counter=0
		for line in check1:
			counter+=1
			linetest = line.split(",")
			if len(line)!=0:
				if len(linetest)!=4:
					return  jsonify({'message': 'Not valid file', 'Error': 'Problem in line '+str(counter), 'Format' : 'X1,Y1,X2,Y2'})
				else:
					for i in range(4):
						if(not linetest[i].strip().isdigit()):
							return jsonify({'message': 'Not valid file', 'Error': 'Problem in line '+str(counter), 'Format' : 'X1,Y1,X2,Y2'})
	
		with open("data/"+fname ,'w') as file:
			file.write(check)
		global FILE_PUB;
		FILE_PUB = "data/"+fname
      
	return jsonify({'message': 'file uploaded successfully.'})

@app.route("/kill", methods = ['GET', 'POST'])
def killprocess():
	requests.post('http://'+URL_PUB+'/kill')
	os.system('pkill client')
	return jsonify({'message': 'Process killed'})

@app.route("/reset", methods = ['GET', 'POST'])
def reset():

	requests.post('http'+IBM+'://'+URL_PUB+'/kill')
	try:
		requests.post('http'+IBM+'://'+URL_PUB_2+'/kill')
	except:
		pass
	os.system('pkill client')
	return jsonify({'message': 'Reset'})

@app.route('/remove', methods = ['GET', 'POST'])
def remove():
	os.system("rm example.csv cluster.csv minLns.csv threshold.csv")
	return jsonify({'message': 'Files removed'})


@app.route('/check/<e>', methods = ['GET', 'POST'])
def check1(e):
	process = subprocess.Popen('cat cluster.csv | wc -l', shell=True, stdout=subprocess.PIPE)
	out, err = process.communicate()
	if (int(e)==int(str(out.decode("utf-8")))):
		return jsonify({'message': 'Check done'})
	else:
		return jsonify({'message': 'Check failed'})

if __name__ == '__main__':
	os.system("ifconfig")
	app.run(debug=True,host=HOST, port=HTTP_PORT)
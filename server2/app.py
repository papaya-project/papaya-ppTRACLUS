from flask import Flask, escape, request
import subprocess
import os
from flask import jsonify
import time
import requests
app = Flask(__name__)

HOST = '0.0.0.0'
HTTP_PORT = 5555
TCP_PORT = 9999

UPLOAD_DIRECTORY = '/app/config_files'
app = Flask(__name__)


@app.route('/round2/<e>/<path>/<itera>/<ip_pub>/<port_pub>', methods=['GET', 'POST'])
def round2(e,path,itera,ip_pub,port_pub):
	print ('./client -w 1 -e '+str(e)+' -a '+ str(ip_pub)+' -p '+str(port_pub)+' -i '+path+' -l '+itera)
	os.system('./client -w 1 -e '+str(e)+' -a '+ str(ip_pub)+' -p '+str(port_pub)+' -i '+path+' -l '+itera)

	return jsonify({'message': 'Round2 started'})

@app.route('/receive/<filenumber>', methods=['GET', 'POST'])
def receivefiles(filenumber):
	print ('python3 server.py 0.0.0.0 ' +filenumber)
	os.system('python3 server.py 0.0.0.0 ' +filenumber)
	return jsonify({'message': 'receive started'})


@app.route('/send/<filenumber>', methods=['GET', 'POST'])
def sendfiles(filenumber):
	print ('python3 server1.py 0.0.0.0 9999 '+filenumber)
	os.system('python3 server1.py 0.0.0.0 9999 '+filenumber)

	return jsonify({'message': 'Sending started'})

@app.route("/kill", methods=['GET', 'POST'])
def killp():
	os.system('pkill client')
	return jsonify({'message': 'Process killed'})

@app.route('/remove', methods = ['GET', 'POST'])
def remove():
	os.system("rm example.csv cluster.csv minLns.csv threshold.csv")
	return jsonify({'message': 'Files removed'})

@app.route('/check/<e>', methods = ['GET', 'POST'])
def check(e):
	process = subprocess.Popen('cat cluster.csv | wc -l', shell=True, stdout=subprocess.PIPE)
	out, err = process.communicate()
	if (int(e)==int(str(out.decode("utf-8")))):
		return jsonify({'message': 'Check done'})
	else:
		return jsonify({'message': 'Check failed'})

if __name__ == '__main__':
	os.system("ifconfig")
	app.run(debug=True,host=HOST, port=HTTP_PORT)
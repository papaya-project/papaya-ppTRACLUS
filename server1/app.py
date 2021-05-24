from flask import Flask, escape, request
import subprocess
import os
from flask import jsonify
import sys
import requests
from werkzeug.utils import secure_filename


app = Flask(__name__)

HOST = '0.0.0.0'
HTTP_PORT = 5555
TCP_PORT = 9999

UPLOAD_DIRECTORY = '/app/config_files'
app = Flask(__name__)


@app.route('/start/<e>', methods=['GET', 'POST'])
def startserver(e):
	print ('./server -w 0 -e '+str(e)+' -a '+ str(HOST)+' -p '+str(TCP_PORT))
	os.system('./server -w 0 -e '+str(e)+' -a '+ str(HOST)+' -p '+str(TCP_PORT))
	return jsonify({'message': 'Round1 Finished'})

@app.route('/round2/<e>/<path>/<itera>', methods=['GET', 'POST'])
def round2(e,path,itera):
	print ('./server -w 1 -e '+str(e)+' -a '+ str(HOST)+' -p '+str(TCP_PORT)+' -i '+path+' -l '+itera)
	process = subprocess.Popen('./server -w 1 -e '+str(e)+' -a '+ str(HOST)+' -p '+str(TCP_PORT)+' -i '+path+' -l '+itera, shell=True, stdout=subprocess.PIPE)
	out, err = process.communicate()
	print("##########")

	print(str(out.decode("utf-8")))

	return jsonify({'message': 'Round2 Finished'})


@app.route('/round3/<e>/<path>', methods=['GET', 'POST'])
def round3(e,path):
	print ('./server -w 2 -e '+str(e)+' -a '+ str(HOST)+' -p '+str(TCP_PORT)+' -i '+path)
	os.system('./server -w 2 -e '+str(e)+' -a '+ str(HOST)+' -p '+str(TCP_PORT)+' -i '+path)

	return jsonify({'message': 'Round3 Finished'})


@app.route("/kill", methods=['GET', 'POST'])
def killp():
	os.system('pkill server')
	return jsonify({'message': 'Process killed'})

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

@app.route('/uploaderr', methods = ['GET', 'POST'])
def upload_file1():
	if request.method == 'POST':
		f = request.files['file']
		fname = secure_filename(f.filename);
      
		check = str(f.read(), 'utf-8')
		check1 = check.split('\n')
		counter=0
		with open(fname ,'w') as file:
			file.write(check)
		global FILE_PUB;
		FILE_PUB = fname
      
	return jsonify({'message': 'file uploaded successfully.'})

@app.route('/check/<e>', methods = ['GET', 'POST'])
def check(e):
	process = subprocess.Popen('cat cluster.csv | wc -l', shell=True, stdout=subprocess.PIPE)
	out, err = process.communicate()
	#print("HEEEEEEEEEEEEEEEERE : "+str(int(str(out.decode("utf-8")))))

	if (int(e)==int(str(out.decode("utf-8")))):
		return jsonify({'message': 'Check done'})
	else:
		return jsonify({'message': 'Check failed'})

@app.route('/remove', methods = ['GET', 'POST'])
def remove():
	os.system("rm example.csv cluster.csv minLns.csv threshold.csv")
	return jsonify({'message': 'Files removed'})

if __name__ == '__main__':
	os.system("ifconfig")
	app.run(debug=True,host=HOST, port=HTTP_PORT)
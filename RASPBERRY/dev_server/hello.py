from flask import Flask, jsonify, render_template, abort, request
import serial
import json
from time import sleep

app = Flask(__name__)

@app.route('/')
def hello_world():
    return 'Hello, World!\n'

welcome = "Welcome to 3ESE API!"

@app.route('/api/welcome/')
def api_welcome():
        return welcome

@app.route('/api/welcome/<int:index>', methods=['GET','POST'])
def api_welcome_index(index):
        if index >= len(welcome):
                abort(404)
        return jsonify({"index": index, "val": welcome[index]})

@app.errorhandler(404)
def page_not_found(error):
        return render_template('page_not_found.html'), 404

@app.route('/api/request/', methods=['GET', 'POST'])
@app.route('/api/request/<path>', methods=['GET','POST'])
def api_request(path=None):
    resp = {
            "method":   request.method,
            "url" :  request.url,
            "path" : path,
            "args": request.args,
            "headers": dict(request.headers),
    }
    if request.method == 'POST':
        resp["POST"] = {
                "data" : request.get_json(),
                }
    return jsonify(resp)


ser = serial.Serial("/dev/ttyAMA0",115200)

temperature = [""]*10

# Create - Retrieve new temperature
@app.route('/api/temp/', methods=['POST'])
def retrieveNewTemp():
        # envoyer GET_T
        ser.write("GET_T\r\n".encode())
        sleep(0.5)
        #Stocker la temperature
        for i in range(8,-1,-1):
                temperature[i+1] = temperature[i]
        temperature[0] = ser.read(12)

        return temperature[0]


pressure = [""]*10

# Create - Retrieve new pression
@app.route('/api/pres/', methods=['POST'])
def retrieveNewPressure():
        #envoyer GET_P
        ser.write("GET_P\r\n".encode())
        sleep(0.5)
        #Stocker la pression
        for i in range(8,-1,-1):
                pressure[i+1] = pressure[i]
        pressure[0] = ser.read(12)
        return pressure[0]


# Update - Change scale (K) for x
@app.route('/api/scale/<int:K>', methods=['POST'])
def set_K(K):
	buffer = "SET_K="+str(K)
	ser.write(buffer.encode())
	return buffer


# Retrieve - Return all previous temperatures
# Ne fonctionne pas
@app.route('/api/temp/', methods=['GET'])
def returnPreviousTemp():
        resp = {
                "temp" : temperature[0],
        }
        #return jsonify(resp)
        return temperature[0]

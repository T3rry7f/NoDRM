#!/usr/local/bin/python
#coding: utf-8
__author__ = 'T3rry'

from threading import Lock
from flask import Flask, render_template, session, request, \
    copy_current_request_context
from flask_socketio import SocketIO, emit, join_room, leave_room, \
    close_room, rooms, disconnect
import wv
from time import sleep
import queue
import logging

log = logging.getLogger('werkzeug')
log.setLevel(logging.ERROR)


cdm=None
async_mode = None
is_busy=False
tasks = queue.Queue()

app = Flask(__name__)
app.config['SECRET_KEY'] = 'secret!'
socketio = SocketIO(app, async_mode=async_mode)
thread = None
thread_lock = Lock()


def background_thread():
    """Example of how to send server generated events to clients."""
    count = 0
    while True:
        socketio.sleep(10)
        count += 1
        socketio.emit('my_response',
                      {'data': 'Server generated event', 'count': count})

@app.route('/')
def index():
    return render_template('index.html', async_mode=socketio.async_mode)

@socketio.event
def init_event(message):
    session['receive_count'] = session.get('receive_count', 0) + 1

    print(message['data'])
    emit('server_response',
         {'data': message['data']})

@socketio.event
def set_license_url_event(message):
    global cdm
    print(f"License servre : {message['data']}")
    cdm=wv.WidevineCDM(message['data'])
    cert=cdm.GetServerCertificate()
    emit('set_serverificate_event',
         {'data': cert})

@socketio.event
def get_pssh_event(message):
    global cdm,is_busy
    links=message['data'].split('\n')

    pssh_type=message['pssh_type']

    cdm.pssh_type=pssh_type

    for link in links:
        if link:
            tasks.put(link)

    while True:
        
        if tasks.empty()==False:
    
            if is_busy==False:
                link=tasks.get()
                print(f"Link:{link}")
                is_busy=True
                resp=cdm.Download(link)
                cdm.ParsePSSH(resp)
                emit('generate_lic_request_event',
                    {'data': cdm.pssh_data}) 
                sleep(0.1)
        else:
            break


@socketio.event
def get_license_event(message):
    global cdm
    lic_request_data=message['data']
    enc_session_key=cdm.GetSessionKey(lic_request_data)
    #print("enc_session_key: ",enc_session_key)
    emit('decrypt_sessionkey_event',
          {'data': enc_session_key}) 

@socketio.event
def get_contentkey_event(message):
    global cdm,is_busy
    cdm.sessionKey=(message['data'])
    keys=cdm.GetContentKey2()
    #print("enc_session_key: ",enc_session_key)
    emit('get_contentkey_event',
          {'data': keys}) 
    is_busy=False



@socketio.event
def disconnect_request():
    @copy_current_request_context
    def can_disconnect():
        disconnect()

    session['receive_count'] = session.get('receive_count', 0) + 1
    emit('my_response',
         {'data': 'Disconnected!', 'count': session['receive_count']},
         callback=can_disconnect)

@socketio.event
def my_ping():
    emit('my_pong')

@socketio.event
def connect():
    global thread
    with thread_lock:
        if thread is None:
            thread = socketio.start_background_task(background_thread)
    emit('my_response', {'data': 'Connected', 'count': 0})


@socketio.on('disconnect')
def test_disconnect():
    print('Client disconnected', request.sid)

if __name__ == '__main__':
    socketio.run(app,host='0.0.0.0',debug=False)



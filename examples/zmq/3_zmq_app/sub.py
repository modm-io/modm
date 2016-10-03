#!/usr/bin/env python

import time
import zmq

def subscriber():
	ctx = zmq.Context()
	zmq_socket = ctx.socket(zmq.SUB)
	zmq_socket.connect('tcp://127.0.0.1:8211')
	zmq_socket.setsockopt(zmq.SUBSCRIBE, '')

	while True:
		string = zmq_socket.recv()
		print('Subscriber received: >>%s<<' % string.encode("hex"))

if __name__ == "__main__":
	subscriber()

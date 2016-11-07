#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Copyright (c) 2016, Sascha Schade
#
# This file is part of the modm project.
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.
# -----------------------------------------------------------------------------

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

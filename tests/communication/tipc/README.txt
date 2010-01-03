These are programs to be used to test the tipc communication.

raw_tipc_transmitter:
	Sends an ACTION and an EVENT each sencond with a running counter as payload.
raw_tipc_receiver:
	Receives the ACTION and EVENT from the raw_tipc_transmitter.

tipc_transmitter:
	Sends an ACTION and an EVENT each sencond with a running counter as payload.
	Receives the ACK to an ACTION.
tipc_receiver:
	Receives the ACTION and EVENT from the raw_tipc_transmitter.
	Sends an ACK back on an ACTION.


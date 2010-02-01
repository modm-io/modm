// create a new 20 ms timeout
Timeout<> delay(20);

while (!delay.isExpired()) {
	// wait until the 20 ms have passed
}

delay.restart(40);	// reset to 40 ms

delay.stop();		// stop timer

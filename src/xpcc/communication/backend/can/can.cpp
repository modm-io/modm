
#include <stdlib.h>

#include "can.hpp"

xpcc::CanInterface::CanInterface() :
sendList(0), receivingMessages(0), receivedMessages(0) {
	
}

xpcc::CanInterface::~CanInterface() {
	
}

void
xpcc::CanInterface::sendPacket(const Header &header, SmartPointer payload)
{
	if (0)
	{
		// TODO: check if the send buffer is empty, so that we can send the
		// message directly without putting it to the list
	}
	else
	{
		SendListItem *message = new SendListItem(header, payload);
		
		// append message to the list of pending messages
		if (sendList == 0) {
			sendList = message;
		}
		else {
			SendListItem *node = sendList;
			while (node->next != 0) {
				node = node->next;
			}
			node->next = message;
		}
	}
}

void
xpcc::CanInterface::dropPacket()
{
	ReceiveListItem *temp = receivedMessages;
	receivedMessages = receivedMessages->next;
	delete temp;
}

void
xpcc::CanInterface::update() {
	checkAndReceiveMessages();
	sendWaitingMessages();
}

void
xpcc::CanInterface::sendWaitingMessages()
{
	if (sendList == 0) {
		// no message in the queue
		return;
	}
	
	uint8_t size = sendList->payload.getSize();
	bool sendFinished = true;
	
	if (size > 8)
	{
		// fragmented message
		uint8_t data[8];
		
		data[0] = sendList->fragmentIndex;
		data[1] = size;
		
		if (size > 6) {
			size = 6;
			sendFinished = false;
		}
		
		memcpy(data + 2, sendList->payload.getPointer(), size);
		
		if (sendCanMessage(sendList->header, data, size)) {
			sendList->fragmentIndex++;
		}
		else {
			sendFinished = false;
		}
	}
	else
	{
		if (!sendCanMessage(sendList->header, sendList->payload.getPointer(), size)) {
			sendFinished = false;
		}
	}
	
	if (sendFinished)
	{
		// message was the last fragment
		// => remove it from the list
		SendListItem *node = sendList;
		sendList = sendList->next;
		delete node;
	}
}

void
xpcc::CanInterface::checkAndReceiveMessages()
{
	while (isCanMessageAvailable())
	{
		if (retrieveCanMessage()) {
			readMessage();
		}
	}
}

void
xpcc::CanInterface::readMessage()
{
	//uint32_t identifier = getCanIdentifier();
	//uint8_t size = getCanSize();
	
	if (receivingMessages == 0)
	{
		//receivingMessages = 
	}
	else {
		/*ReceiveListItem *node = receivingMessages;
		do {
			if (header == node->header &&
				size == node->size)
			{
				
			}
			node = node->next;
		} while (node);
		*/
	}
}

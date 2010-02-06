// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2009, Roboterclub Aachen e.V.
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Roboterclub Aachen e.V. nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY ROBOTERCLUB AACHEN E.V. ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ROBOTERCLUB AACHEN E.V. BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 * $Id$
 */
// ----------------------------------------------------------------------------

#ifndef	XPCC__LIST_HPP
	#error	"Don't include this file directly, use 'list.hpp' instead"
#endif

// ----------------------------------------------------------------------------
template <typename T>
xpcc::List<T>::List() : 
	first(0), last(0)
{
}

// ----------------------------------------------------------------------------
template <typename T>
void
xpcc::List<T>::prepend(Node *node)
{
	if (first == 0) {
		first = node;
		last = node;
	}
	else {
		node->next = first;
		first = node;
	}
}

template <typename T>
void
xpcc::List<T>::append(Node *node)
{
	if (first == 0) {
		first = node;
		last = node;
	}
	else {
		last->next = node;
		last = node;
	}
}

template <typename T>
void
xpcc::List<T>::insertAfter(Node *previous, Node *newNode)
{
	newNode->next = previous->next;
	previous->next = newNode;
}

template <typename T>
typename xpcc::List<T>::Node*
xpcc::List<T>::remove(Node *node)
{
	if (node == first)
	{
		first = node->next;
		if (first == 0) {
			// list is now empty
			last = 0;
		}
		
		node->next = 0;
		return node;
	}
	
	Node *c = first;
	do {
		if (c->next == node) {
			c->next = node->next;
			
			if (c->next == 0) {
				last = c;
			}
			
			node->next = 0;
			return node;
		}
	}
	while ((c = c->next));
	
	return 0;
}

template <typename T>
typename xpcc::List<T>::Node*
xpcc::List<T>::removeAfter(Node *previous)
{
	Node *node = previous->next;
	if (node == 0) {
		return 0;
	}
	previous->next = node->next;
	node->next = 0;
	
	if (node == last) {
		last = previous;
	}
	return node;
}

template <typename T>
typename xpcc::List<T>::Node*
xpcc::List<T>::removeFirst()
{
	Node *node = first;
	first = first->next;
	if (first == 0) {
		// list is now empty
		last = 0;
	}
	else {	
		node->next = 0;
	}
	return node;
}

template <typename T>
typename xpcc::List<T>::Node*
xpcc::List<T>::at(int index) const
{
	Node *node = first;
	for (int i = 0; i < index; i++)
	{
		if (node == 0) {
			break;
		}
		node = node->next;
	}
	return node;
}

// ----------------------------------------------------------------------------
template <typename T>
bool
xpcc::List<T>::isEmpty() const
{
	return (first == 0);
}

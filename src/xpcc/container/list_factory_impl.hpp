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
template<typename T>
template<int N>
xpcc::List<T>::NodeFactory<N>::NodeFactory()
{
	int_fast16_t i;
	for (i = 0; i < N - 1; ++i) {
		pool[i].next = &pool[i + 1];
	}
	first = &pool[0];
}

template<typename T>
template<int N>
typename xpcc::List<T>::Node *
xpcc::List<T>::NodeFactory<N>::getNode()
{
	Node *node = first;
	first = first->next;
	return node;
}

template<typename T>
template<int N>
typename xpcc::List<T>::Node *
xpcc::List<T>::NodeFactory<N>::getNode(const T& value)
{
	Node *node = first;
	first = first->next;
	
	node->setValue(value);
	return node;
}

template<typename T>
template<int N>
void
xpcc::List<T>::NodeFactory<N>::freeNode(Node *node)
{
	node->next = first;
	first = node;
}

template<typename T>
template<int N>
bool
xpcc::List<T>::NodeFactory<N>::isEmpty() const
{
	return (first == 0);
}

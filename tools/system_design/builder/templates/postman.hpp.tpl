%# Copyright (c) 2010-2011, Fabian Greif
%# Copyright (c) 2013, 2015, Niklas Hauser
%# Copyright (c) 2015, Sascha Schade
%#
%# This file is part of the modm project.
%#
%# This Source Code Form is subject to the terms of the Mozilla Public
%# License, v. 2.0. If a copy of the MPL was not distributed with this
%# file, You can obtain one at http://mozilla.org/MPL/2.0/.
%# ----------------------------------------------------------------------------
/*
 * WARNING: This file is generated automatically from postman.hpp.tpl
 * Do not edit! Please modify the corresponding XML file instead.
 */
// ----------------------------------------------------------------------------

#ifndef	POSTMAN_HPP
#define	POSTMAN_HPP

#include <modm/communication/xpcc.hpp>
#include <modm/communication/xpcc/postman/postman.hpp>
#include "packets.hpp"

class Postman : public xpcc::Postman
{
public:
	xpcc::Postman::DeliverInfo
	deliverPacket(const xpcc::Header& header, const modm::SmartPointer& payload);

	bool
	isComponentAvailable(uint8_t component) const;

	void
	update();

{%- if resumables > 0 %}
private:
	struct
	ActionBuffer
	{
		ActionBuffer()
		: destination(0) {}

		ActionBuffer(const xpcc::Header& header)
		:	response(header), destination(header.destination) {}

		void
		remove()
		{ destination = 0; }

		xpcc::ResponseHandle response;	// 2B
		uint8_t destination;			// 1B
	};	// 3B

	static constexpr uint8_t resumableActions = {{ resumables }};
	ActionBuffer actionBuffer[resumableActions];

	{%- if resumablePayloads > 0 %}
	struct
	PayloadBuffer
	{
		PayloadBuffer()
		{}

		PayloadBuffer(const modm::SmartPointer& payload)
		:	payload(payload) {}

		void
		remove()
		{ payload = modm::SmartPointer(); }

		modm::SmartPointer payload;
	};	// 2B (AVR), 4B (ARM) + 3B HEAP

	static constexpr uint8_t resumablePayloads = {{ resumablePayloads }};
	PayloadBuffer payloadBuffer[resumablePayloads];
	{%- endif %}
{%- endif %}

private:
{%- for component in components %}
	{%- for action in component.actions %}
		{%- if action.call == "resumable" %}
	uint8_t
			{%- if action.parameterType != None %}
				{%- set typePrefix = "" if action.parameterType.isBuiltIn else namespace ~ "::packet::" %}
				{%- set arguments = ", const " ~ typePrefix ~ (action.parameterType.name | CamelCase) ~ "& payload" %}
			{%- else %}
				{%- set arguments = "" %}
			{%- endif %}
	component_{{ component.name | camelCase }}_action{{ action.name | CamelCase }}(const xpcc::ResponseHandle& response{{ arguments }});
		{%- endif %}
	{%- endfor %}
{%- endfor %}
};

#endif	// POSTMAN_HPP

// ----------------------------------------------------------------------------
/*
 * WARNING: This file is generated automatically from postman.hpp.tpl
 * Do not edit! Please modify the corresponding XML file instead.
 */
// ----------------------------------------------------------------------------

{#- Calculates the number of coroutines with and without arguments, hacky #}
{%- set ac = [] %}
{%- for component in components %}
	{%- for action in component.actions %}
		{%- if action.call == "coroutine" and action.parameterType != None %}
			{%- if ac.append(1)%}{%- endif %}
		{%- endif %}
	{%- endfor %}
{%- endfor %}
{%- set coAcLen = ac.__len__() %}

{%- set ac = [] %}
{%- for component in components %}
	{%- for action in component.actions %}
		{%- if action.call == "coroutine" and action.parameterType == None %}
			{%- if ac.append(1)%}{%- endif %}
		{%- endif %}
	{%- endfor %}
{%- endfor %}
{%- set coAcNoArgLen = ac.__len__() %}

#ifndef	POSTMAN_HPP
#define	POSTMAN_HPP

#include <xpcc/communication.hpp>
#include <xpcc/communication/xpcc/postman/postman.hpp>
#include "packets.hpp"

class Postman : public xpcc::Postman
{
public:
	xpcc::Postman::DeliverInfo
	deliverPacket(const xpcc::Header& header, const xpcc::SmartPointer& payload);

	bool
	isComponentAvailable(uint8_t component) const;

	void
	update();

private:
{%- if coAcLen > 0 %}
	// Coroutine Actions with Arguments
	struct
	ActionBuffer
	{
		ActionBuffer()
		: destination(0) {}

		ActionBuffer(const xpcc::Header& header, const xpcc::SmartPointer& payload)
		:	payload(payload), response(header), destination(header.destination)
		{
		}

		void
		remove()
		{
			destination = 0;
			payload = xpcc::SmartPointer();
		}

		xpcc::SmartPointer payload;		// 2B (AVR), 4B (ARM)
		xpcc::ResponseHandle response;	// 2B
		uint8_t destination;			// 1B
	};	// 5B (AVR), 7B(ARM)

	static constexpr uint8_t coroutineActions = {{ coAcLen }};
	ActionBuffer actionBuffer[coroutineActions];

	bool
	pushIntoBuffer(const xpcc::Header& header, const xpcc::SmartPointer& payload);
{%- endif %}
{% if coAcNoArgLen > 0 %}
	// Coroutine Actions without Arguments
	struct
	ActionBufferNoArg
	{
		ActionBufferNoArg()
		: destination(0) {}

		ActionBufferNoArg(const xpcc::Header& header)
		: response(header), destination(header.destination) {}

		void
		remove()
		{ destination = 0; }

		xpcc::ResponseHandle response;	// 2B
		uint8_t destination;			// 1B
	};	// 3B

	static constexpr uint8_t coroutineActionsNoArg = {{ coAcNoArgLen }};
	ActionBufferNoArg actionBufferNoArg[coroutineActionsNoArg];
{%- endif %}

private:
{%- for component in components %}
	{%- for action in component.actions %}
		{%- if action.call == "coroutine" %}
	uint8_t
			{%- if action.parameterType != None %}
				{%- set typePrefix = "" if action.parameterType.isBuiltIn else "robot::packet::" %}
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

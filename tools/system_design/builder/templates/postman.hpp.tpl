// ----------------------------------------------------------------------------
/*
 * WARNING: This file is generated automatically, do not edit!
 * Please modify the corresponding XML file instead.
 */
// ----------------------------------------------------------------------------

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
	isComponentAvaliable(uint8_t component) const;

	void
	update();

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

// ----------------------------------------------------------------------------
/*
 * WARNING: This file is generated automatically, do not edit!
 * Please modify the corresponding XML file instead.
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

{% for component in components %}
#include "component_{{ component.name | camelcase }}/{{ component.name | camelcase }}.hpp"
{%- endfor %}

#include "identifier.hpp"
#include "postman.hpp"

namespace component
{
	{%- for component in components %}
	extern {{ component.name | CamelCase }}	{{ component.name | camelCase }};
	{%- endfor %}
}

// ----------------------------------------------------------------------------
xpcc::Postman::DeliverInfo
Postman::deliverPacket(const xpcc::Header& header, const xpcc::SmartPointer& payload)
{
	xpcc::ResponseHandle response(header);

	// Avoid warnings about unused variables
	(void) payload;
	(void) response;

	switch (header.destination)
	{
{%- set actionNumber = 0 %}
{%- set actionNumberNoArg = 0 %}
{%- for component in components %}
		case robot::component::{{ component.name | CAMELCASE }}:
		{
			switch (header.packetIdentifier)
			{
	{%- for action in component.actions %}
		{%- if action.parameterType != None %}
			{%- set typePrefix = "" if action.parameterType.isBuiltIn else "robot::packet::" %}
			{%- set payload = ", payload.get<" ~ typePrefix ~ (action.parameterType.name | CamelCase) ~ ">()" %}
			{%- set arguments = "const " ~ typePrefix ~ (action.parameterType.name | CamelCase) ~ "& payload" %}
			{%- set pointer = ", payload" %}
		{%- else %}
			{%- set payload = "" %}
			{%- set arguments = "" %}
			{%- set pointer = "" %}
		{%- endif %}
				case robot::action::{{ action.name | CAMELCASE }}:
		{%- if action.call == "coroutine" %}
			{%- if action.parameterType != None %}
					if (actionBuffer[{{ actionNumber }}].destination != 0) {
			{%- else %}
					if (actionBufferNoArg[{{ actionNumber }}].destination != 0) {
			{%- endif %}
						component::{{component.name | camelCase}}.getCommunicator()->sendNegativeResponse(response);
					}
					else if (component_{{ component.name | camelCase }}_action{{ action.name | CamelCase }}(response{{ payload }}) > xpcc::co::NestingError) {
			{%- if action.parameterType != None %}
						actionBuffer[{{ actionNumber }}] = ActionBuffer(header, payload);
						{%- set actionNumber = actionNumber + 1 %}
			{%- else %}
						actionBufferNoArg[{{ actionNumber }}] = ActionBufferNoArg(header);
						{%- set actionNumberNoArg = actionNumberNoArg + 1 %}
			{%- endif %}
					}
		{%- else %}
			{%- if action.parameterType != None %}
				{%- set payload = ", &payload.get<" ~ typePrefix ~ (action.parameterType.name | CamelCase) ~ ">()" %}
				{%- set arguments = ", const " ~ typePrefix ~ (action.parameterType.name | CamelCase) ~ " *payload" %}
			{%- endif %}
					// void action{{ action.name | CamelCase }}(const xpcc::ResponseHandle& responseHandle{{ arguments }});
					component::{{ component.name | camelCase }}.action{{ action.name | CamelCase }}(response{{ payload }});
		{%- endif %}
					return OK;
	{%- endfor %}

				default:
					return NO_ACTION;
			}
			break;
		}
{% endfor %}

		// Events
		case 0:
			switch (header.packetIdentifier)
			{
{%- for event in container.events.subscribe %}
				case robot::event::{{ event.name | CAMELCASE }}:
	{%- for component in eventSubscriptions[event.name] %}
		{%- if events[event.name].type != None %}
					// void event{{ event.name | CamelCase }}(const xpcc::Header& header, const robot::packet::{{ events[event.name].type.name | CamelCase }} *payload);
					component::{{ component.name | camelCase }}.event{{ event.name | CamelCase }}(header, &payload.get<robot::packet::{{ events[event.name].type.name | CamelCase }}>());
		{%- else %}
					// void event{{ event.name | CamelCase }}(const xpcc::Header& header);
					component::{{ component.name | camelCase }}.event{{ event.name | CamelCase }}(header);
		{%- endif %}
	{%- endfor %}
					break;
{% endfor %}
				default:
					break;
			}
			return OK;

		default:
			return NO_COMPONENT;
	}

	return NOT_IMPLEMENTED_YET_ERROR;
}

// ----------------------------------------------------------------------------
bool
Postman::isComponentAvailable(uint8_t component) const
{
	switch (component)
	{
{%- for component in components %}
		case robot::component::{{ component.name | CAMELCASE }}:
{%- endfor %}
			return true;
			break;

		default:
			return false;
	}
}

void
Postman::update()
{
{%- if coAcLen > 0 %}
	for(ActionBuffer &action : actionBuffer)
	{
		switch (action.destination)
		{
	{%- for component in components %}
		{%- set ac = [] %}
		{%- for action in component.actions %}
			{%- if action.call == "coroutine" and action.parameterType != None %}
				{%- if ac.append(1)%}{%- endif %}
			{%- endif %}
		{%- endfor %}
		{%- set has_actions = ac.__len__() > 0 %}

		{%- if has_actions %}
			case robot::component::{{ component.name | CAMELCASE }}:
			{
				switch (action.response.getIdentifier())
				{
			{%- for action in component.actions %}
				{%- if action.call == "coroutine" and action.parameterType != None %}
					{%- set typePrefix = "" if action.parameterType.isBuiltIn else "robot::packet::" %}
					case robot::action::{{ action.name | CAMELCASE }}:
						if (component_{{ component.name | camelCase }}_action{{ action.name | CamelCase }}(action.response, action.payload.get<{{ typePrefix ~ (action.parameterType.name | CamelCase) }}>()) <= xpcc::co::NestingError) {
							action.remove();
						}
						break;
				{%- endif %}
			{%- endfor %}

					default:
						break;
				}
				break;
			}
		{%- endif %}
	{%- endfor %}

			default:
				break;
		}
	}
{%- endif %}
{%- if coAcNoArgLen > 0 %}
	for(ActionBufferNoArg &action : actionBufferNoArg)
	{
		switch (action.destination)
		{
	{%- for component in components %}
		{%- set ac = [] %}
		{%- for action in component.actions %}
			{%- if action.call == "coroutine" and action.parameterType == None %}
				{%- if ac.append(1)%}{%- endif %}
			{%- endif %}
		{%- endfor %}
		{%- set has_actions = ac.__len__() > 0 %}

		{%- if has_actions %}
			case robot::component::{{ component.name | CAMELCASE }}:
			{
				switch (action.response.getIdentifier())
				{
			{%- for action in component.actions %}
				{%- if action.call == "coroutine" and action.parameterType == None %}
					case robot::action::{{ action.name | CAMELCASE }}:
						if (component_{{ component.name | camelCase }}_action{{ action.name | CamelCase }}(action.response) <= xpcc::co::NestingError) {
							action.remove();
						}
						break;
				{%- endif %}
			{%- endfor %}

					default:
						break;
				}
				break;
			}
		{%- endif %}
	{%- endfor %}

			default:
				break;
		}
	}
{%- endif %}
}

// ----------------------------------------------------------------------------
{%- for component in components %}
	{%- for action in component.actions %}
		{%- if action.call == "coroutine" %}
uint8_t
			{%- if action.parameterType != None %}
				{%- set typePrefix = "" if action.parameterType.isBuiltIn else "robot::packet::" %}
				{%- set arguments = ", const " ~ typePrefix ~ (action.parameterType.name | CamelCase) ~ "& payload" %}
				{%- set payload = "payload" %}
			{%- else %}
				{%- set arguments = "" %}
				{%- set payload = "" %}
			{%- endif %}
Postman::component_{{ component.name | camelCase }}_action{{ action.name | CamelCase }}(const xpcc::ResponseHandle& response{{ arguments }})
{
	auto result = component::{{ component.name | camelCase }}.action{{ action.name | CamelCase }}({{ payload }});
	if (result.state <= xpcc::co::NestingError) {
		if (result.result.response == xpcc::Response::Positive) {
			{%- if action.returnType != None %}
			component::{{component.name | camelCase}}.getCommunicator()->sendResponse(response, result.result.data);
			{%- else %}
			component::{{component.name | camelCase}}.getCommunicator()->sendResponse(response);
			{%- endif %}
		} else {
			component::{{component.name | camelCase}}.getCommunicator()->sendNegativeResponse(response);
		}
	}
	return result.state;
}
		{%- endif %}
	{%- endfor %}
{%- endfor %}

// ----------------------------------------------------------------------------
/*
 * WARNING: This file is generated automatically, do not edit!
 * Please modify the corresponding XML file instead.
 */
// ----------------------------------------------------------------------------

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
{%- set payloadNumber = 0 %}
{%- for component in components %}
		case {{ namespace }}::component::{{ component.name | CAMELCASE }}:
		{
			switch (header.packetIdentifier)
			{
	{%- for action in component.actions %}
		{%- if action.parameterType != None %}
			{%- set typePrefix = "" if action.parameterType.isBuiltIn else namespace ~ "::packet::" %}
			{%- set payload = ", payload.get<" ~ typePrefix ~ (action.parameterType.name | CamelCase) ~ ">()" %}
			{%- set arguments = "const " ~ typePrefix ~ (action.parameterType.name | CamelCase) ~ "& payload" %}
			{%- set pointer = ", payload" %}
		{%- else %}
			{%- set payload = "" %}
			{%- set arguments = "" %}
			{%- set pointer = "" %}
		{%- endif %}
				case {{ namespace }}::action::{{ action.name | CAMELCASE }}:
		{%- if action.call == "coroutine" %}
					if (actionBuffer[{{ actionNumber }}].destination != 0) {
						component::{{component.name | camelCase}}.getCommunicator()->sendNegativeResponse(response);
					}
					else if (component_{{ component.name | camelCase }}_action{{ action.name | CamelCase }}(response{{ payload }}) == xpcc::co::Running) {
						actionBuffer[{{ actionNumber }}] = ActionBuffer(header);
			{%- set actionNumber = actionNumber + 1 %}
			{%- if action.parameterType != None %}
						payloadBuffer[{{ payloadNumber }}] = PayloadBuffer(payload);
				{%- set payloadNumber = payloadNumber + 1 %}
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
				case {{ namespace }}::event::{{ event.name | CAMELCASE }}:
	{%- for component in eventSubscriptions[event.name] %}
		{%- if events[event.name].type != None %}
					// void event{{ event.name | CamelCase }}(const xpcc::Header& header, const {{ namespace }}::packet::{{ events[event.name].type.name | CamelCase }} *payload);
					component::{{ component.name | camelCase }}.event{{ event.name | CamelCase }}(header, &payload.get<{{ namespace }}::packet::{{ events[event.name].type.name | CamelCase }}>());
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
		case {{ namespace }}::component::{{ component.name | CAMELCASE }}:
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
{%- set payloadNumber = 0 %}
{%- if coroutines > 0 %}
	for(ActionBuffer &action : actionBuffer)
	{
		switch (action.destination)
		{
	{%- for component in components %}
		{%- if component.coroutines > 0 %}
			case {{ namespace }}::component::{{ component.name | CAMELCASE }}:
			{
				switch (action.response.getIdentifier())
				{
			{%- for action in component.actions %}
				{%- if action.call == "coroutine" %}
					{%- set payload = "" %}
					{%- if action.parameterType != None %}
						{%- set typePrefix = "" if action.parameterType.isBuiltIn else namespace ~ "::packet::" %}
						{%- set payload = ", payloadBuffer[" ~ payloadNumber ~ "].payload.get<" ~ typePrefix ~ (action.parameterType.name | CamelCase) ~ ">()" %}
					{%- endif %}
					case {{ namespace }}::action::{{ action.name | CAMELCASE }}:
						if (component_{{ component.name | camelCase }}_action{{ action.name | CamelCase }}(action.response{{ payload }}) != xpcc::co::Running) {
							action.remove();
					{%- if action.parameterType != None %}
							payloadBuffer[{{ payloadNumber }}].remove();
						{%- set payloadNumber = payloadNumber + 1 %}
					{%- endif %}
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
				{%- set typePrefix = "" if action.parameterType.isBuiltIn else namespace ~ "::packet::" %}
				{%- set arguments = ", const " ~ typePrefix ~ (action.parameterType.name | CamelCase) ~ "& payload" %}
				{%- set payload = "payload" %}
			{%- else %}
				{%- set arguments = "" %}
				{%- set payload = "" %}
			{%- endif %}
Postman::component_{{ component.name | camelCase }}_action{{ action.name | CamelCase }}(const xpcc::ResponseHandle& response{{ arguments }})
{
	auto result = component::{{ component.name | camelCase }}.action{{ action.name | CamelCase }}({{ payload }});
	if (result.state < xpcc::co::Running) {
		if (result.state == xpcc::co::Stop and result.result.response == xpcc::Response::Positive) {
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

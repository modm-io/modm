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
{%- set actionNumber = [] %}
{%- set payloadNumber = [] %}

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
		{%- if action.returnType != None %}
			{%- set returns = ("" if action.returnType.isBuiltIn else namespace ~ "::packet::") ~ action.returnType.name %}
		{%- else %}
			{%- set returns = "void" %}
		{%- endif %}
				case {{ namespace }}::action::{{ action.name | CAMELCASE }}:
		{%- if action.call == "resumable" %}
					// xpcc::ActionResponse<{{ returns }}> action{{ action.name | CamelCase }}({{ arguments }});
					if (actionBuffer[{{ actionNumber.__len__() }}].destination != 0) {
						component::{{component.name | camelCase}}.getCommunicator()->sendNegativeResponse(response);
					}
					else if (component_{{ component.name | camelCase }}_action{{ action.name | CamelCase }}(response{{ payload }}) == xpcc::rf::Running) {
						actionBuffer[{{ actionNumber.__len__() }}] = ActionBuffer(header);
			{%- if actionNumber.append(1)%}{%- endif %}
			{%- if action.parameterType != None %}
						payloadBuffer[{{ payloadNumber.__len__() }}] = PayloadBuffer(payload);
				{%- if payloadNumber.append(1)%}{%- endif %}
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
{%- set payloadNumber = [] %}
{%- if resumables > 0 %}
	for(ActionBuffer &action : actionBuffer)
	{
		switch (action.destination)
		{
	{%- for component in components %}
		{%- if component.resumables > 0 %}
			case {{ namespace }}::component::{{ component.name | CAMELCASE }}:
			{
				switch (action.response.getIdentifier())
				{
			{%- for action in component.actions %}
				{%- if action.call == "resumable" %}
					{%- set payload = "" %}
					{%- if action.parameterType != None %}
						{%- set typePrefix = "" if action.parameterType.isBuiltIn else namespace ~ "::packet::" %}
						{%- set payload = ", payloadBuffer[" ~ payloadNumber.__len__() ~ "].payload.get<" ~ typePrefix ~ (action.parameterType.name | CamelCase) ~ ">()" %}
					{%- endif %}
					case {{ namespace }}::action::{{ action.name | CAMELCASE }}:
						if (component_{{ component.name | camelCase }}_action{{ action.name | CamelCase }}(action.response{{ payload }}) != xpcc::rf::Running) {
							action.remove();
					{%- if action.parameterType != None %}
							payloadBuffer[{{ payloadNumber.__len__() }}].remove();
						{%- if payloadNumber.append(1)%}{%- endif %}
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
		{%- if action.call == "resumable" %}
uint8_t
			{%- if action.parameterType != None %}
				{%- set typePrefix = "" if action.parameterType.isBuiltIn else namespace ~ "::packet::" %}
				{%- set typePayload = "const " ~ typePrefix ~ (action.parameterType.name | CamelCase) ~ "& payload" %}
				{%- set arguments = ", " ~ typePayload %}
				{%- set payload = "payload" %}
			{%- else %}
				{%- set typePayload = "" %}
				{%- set arguments = "" %}
				{%- set payload = "" %}
			{%- endif %}
			{%- if action.returnType != None %}
				{%- set returns = ("" if action.returnType.isBuiltIn else namespace ~ "::packet::") ~ action.returnType.name %}
			{%- else %}
				{%- set returns = "void" %}
			{%- endif %}
Postman::component_{{ component.name | camelCase }}_action{{ action.name | CamelCase }}(const xpcc::ResponseHandle& response{{ arguments }})
{
	// xpcc::ActionResponse<{{ returns }}> action{{ action.name | CamelCase }}({{ typePayload }});
	xpcc::ActionResponse<{{ returns }}> result = component::{{ component.name | camelCase }}.action{{ action.name | CamelCase }}({{ payload }});
	if (result.getState() < xpcc::rf::Running) {
		if (result.getState() == xpcc::rf::Stop and result.getResult().response == xpcc::Response::Positive) {
			{%- if action.returnType != None %}
			component::{{component.name | camelCase}}.getCommunicator()->sendResponse(response, result.getResult().data);
			{%- else %}
			component::{{component.name | camelCase}}.getCommunicator()->sendResponse(response);
			{%- endif %}
		} else {
			component::{{component.name | camelCase}}.getCommunicator()->sendNegativeResponse(response);
		}
	}
	return result.getState();
}
		{%- endif %}
	{%- endfor %}
{%- endfor %}

// ----------------------------------------------------------------------------
/*
 * WARNING: This file is generated automatically, do not edit!
 * Please modify the corresponding XML file instead.
 */
// ----------------------------------------------------------------------------

#include "postman.hpp"

{%- for component in container.components %}
#include "component_{{ component.name | camelcase }}/{{ component.name | camelcase }}.hpp"
{%- endfor %}

namespace component
{
	{% for component in container.components -%}
	extern {{ component.name | CamelCase }}	{{ component.name | camelCase }};
	{% endfor %}
}

// ----------------------------------------------------------------------------
xpcc::Postman::DeliverInfo
Postman::deliverPacket(const xpcc::Header& header, const xpcc::SmartPointer& payload)
{
	const xpcc::ResponseHandle& handle(header);
	
	switch (header.destination)
	{
{%- for component in container.components %}
		case robot::component::{{ component.name | CAMELCASE }}:
			switch (header.packetIdentifier)
			{
	{%- for action in component.actions %}
				case robot::action::{{ action.name | CAMELCASE }}:
		{%- if action.parameterType != None %}
					component::{{ component.name | camelCase }}.action{{ action.name | CamelCase }}(handle, &payload.get<robot::packet::{{ action.parameterType | CamelCase }}>());
		{%- else %}
					component::{{ component.name | camelCase }}.action{{ action.name | CamelCase }}(handle);
		{%- endif %}
					return OK;
	{%- endfor %}
				
				default:
					return NO_ACTION;
			}
			break;	
{% endfor %}
		
		// Events
		case 0:
			switch (header.packetIdentifier)
			{
{%- for event in container.events.subscribe %}
				case robot::event::{{ event.name | CAMELCASE }}:
	{%- for component in eventSubscriptions[event.name] %}
		{%- if events[event.name].type != None %}
					component::{{ component.name | camelCase }}.event{{ event.name | CamelCase }}(header, &payload.get<robot::packet::{{ events[event.name].type | CamelCase }}>());
		{%- else %}
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
Postman::isComponentAvaliable(const xpcc::Header& header) const
{
	switch (header.destination)
	{
{%- for component in container.components %}
		case robot::component::{{ component.name | CAMELCASE }}:
{%- endfor %}
			return true;
			break;
		
		default:
			return false;
	}
}

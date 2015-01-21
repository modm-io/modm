/*
 * WARNING: This file is generated automatically from cpp_communication.tpl.
 * Do not edit! Please modify the corresponding XML file instead.
 */
// ----------------------------------------------------------------------------

#ifndef ROBOT__CPP_COMMUNICATION_HPP
#define ROBOT__CPP_COMMUNICATION_HPP

#include "identifier.hpp"
#include "packets.hpp"
#include <xpcc/communication/xpcc/communicator.hpp>

namespace robot
{

	/** Using this class you can publish events.*/
	class EventPublisher
	{
	public:
		
	{%- for event in events.iter() %}
		{% if event.type -%}
		{% if event.description %}/** {{ event.description | xpcc.wordwrap(72) | xpcc.indent(2) }}
		* \param const robot::packet::{{ event.type.name | CamelCase -}}& */{% endif %}
		static inline void
		{{ event.name | camelCase }}(
				xpcc::Communicator *communicator,
				
				{%- if event.type.isBuiltIn %}
				const {{ event.type.name | CamelCase }}& packet)
				{%- else %}
				const robot::packet::{{ event.type.name | CamelCase }}& packet)
				{%- endif %}
		{
			communicator->publishEvent(
				robot::event::Identifier::{{ event.name | CAMELCASE }},
				packet);
		}
		{% else -%}
		{% if event.description %}/** {{ event.description | xpcc.wordwrap(72) | xpcc.indent(2) }}*/{% endif %}
		static inline void
		{{ event.name | camelCase }}(xpcc::Communicator *communicator) {
			communicator->publishEvent(
				robot::event::Identifier::{{ event.name | CAMELCASE }});
		}
		{% endif -%}
	{% endfor %}
	};

{%- for component in components.iter() %}
	
	{% if component.description %}/** {{ component.description | xpcc.wordwrap(72) | xpcc.indent(2) }} */{% endif %}
	class {{ component.name | CamelCase }}
	{
	public:
		{% for action in component.flattened().actions %}
		{% if action.description -%}
		{% if action.parameterType -%}
		/** {{ action.description | xpcc.wordwrap(72) | xpcc.indent(2) }}
		\param packet robot::packet::{{ action.parameterType.flattened().name | CamelCase }}&
		 */
		{% else -%}
		/** {{ action.description | xpcc.wordwrap(72) | xpcc.indent(2) }} */
		{% endif -%}
		{% else -%}
		{% if action.parameterType -%}
		/**
		\param packet robot::packet::{{ action.parameterType.flattened().name | CamelCase }}&
		 */
		{% endif -%}
		{% endif -%}
		{% if action.parameterType -%}
		static inline void
		{{ action.name | camelCase }} (
				xpcc::Communicator *communicator,
				{%- if action.parameterType.isBuiltIn %}
				const {{ action.parameterType.name | CamelCase }}& packet)
				{%- else %}
				const robot::packet::{{ action.parameterType.name | CamelCase }}& packet)
				{%- endif %}
		{
			communicator->callAction(
				robot::component::Identifier::{{ component.name | CAMELCASE }},
				robot::action::Identifier::{{ action.name | CAMELCASE }},
				packet);
		}
		static inline void
		{{ action.name | camelCase }} (
				xpcc::Communicator *communicator,
				{%- if action.parameterType.isBuiltIn %}
				const {{ action.parameterType.name | CamelCase }}& packet,
				{%- else %}
				const robot::packet::{{ action.parameterType.name | CamelCase }}& packet,
				{%- endif %}
				xpcc::ResponseCallback& responseCallback)
		{
			communicator->callAction(
				robot::component::Identifier::{{ component.name | CAMELCASE }},
				robot::action::Identifier::{{ action.name | CAMELCASE }},
				packet,
				responseCallback);
		}
		{% else -%}
		static inline void
		{{ action.name | camelCase }} (xpcc::Communicator *communicator) {
			communicator->callAction(
				robot::component::Identifier::{{ component.name | CAMELCASE }},
				robot::action::Identifier::{{ action.name | CAMELCASE }});
		}
		static inline void
		{{ action.name | camelCase }} (xpcc::Communicator *communicator, xpcc::ResponseCallback& responseCallback) {
			communicator->callAction(
				robot::component::Identifier::{{ component.name | CAMELCASE }},
				robot::action::Identifier::{{ action.name | CAMELCASE }},
				responseCallback);
		}
		{% endif %}
		{%- endfor %}
		
	};

	
{%- endfor %}

} // namespace robot

#endif // ROBOT__CPP_COMMUNICATION_HPP

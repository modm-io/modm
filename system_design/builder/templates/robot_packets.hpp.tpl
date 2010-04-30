// ----------------------------------------------------------------------------
/*
 * WARNING: This file is generated automatically, do not edit!
 * Please modify the corresponding XML file instead.
 */
// ----------------------------------------------------------------------------

#ifndef	ROBOT__PACKETS_HPP
#define	ROBOT__PACKETS_HPP

#include <stdint.h>

namespace robot
{
	namespace packet
	{
{%- for packet in packets %}
		{% if packet.description %}/** {{ packet.description | xpcc.wordwrap(68) | xpcc.indent(2) }} */{% endif %}
	{%- if packet.isEnum %}
		enum {{ packet.name | typeName }}
		{
			{%- for element in packet.iter() %}
			{{ element.name | enumElement }} = {{ element.value }},{% if element.description %}	///< {{ element.description }}{% endif %}
			{%- endfor %}
		} __attribute__((packed));
	{% elif packet.isStruct %}
		struct {{ packet.name | typeName }}
		{
			{{ packet | generateConstructor }};
			
			{{ packet | generateConstructor(default=False) }};
			
			{%- for element in packet.iter() %}
			{{ element | subtype }};
			{%- endfor %}
		} __attribute__((packed));
	{% elif packet.isTypedef %}
		typedef {{ packet.type.name | typeName }} {{ packet.name | typeName }};
	{% endif %}
{%- endfor -%}
	} // namespace packet
} // namespace robot

#endif	// ROBOT__PACKETS_HPP

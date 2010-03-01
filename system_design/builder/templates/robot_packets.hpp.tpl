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
{%- for type in types %}
		{% if type.desc %}/** {{ type.desc | xpcc.wordwrap(68) | xpcc.indent(2) }} */{% endif %}
	{%- if type.is_enum %}
		enum {{ type.name | cpp.type }}
		{
			{%- for element in type.iter() %}
			{{ element.name }} = {{ element.value }},{% if element.desc %}	///< {{ element.desc }}{% endif %}
			{%- endfor %}
		};
	{% elif type.is_struct %}
		struct {{ type.name | cpp.type }}
		{
			{{ type | cpp.constructor }};
			
			{{ type | cpp.constructor(default=False) }};
			
			{%- for element in type.iter() %}
			{{ element | cpp.subtype }};
			{%- endfor %}
		} __attribute__((packed));
	{% elif type.is_typedef %}
		typedef {{ type.type.name | cpp.type }} {{ type.name | cpp.type }};
	{% endif %}
{%- endfor -%}
	} // namespace packet
} // namespace robot

#endif	// ROBOT__PACKETS_HPP

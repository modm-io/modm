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
{%- if packet.isBuiltIn %}{% continue %}{% endif %}
		{%- if packet.description %}
		/** {{ packet.description | xpcc.wordwrap(67) | xpcc.indent(2, " * ") }} */
		{%- endif %}
	{%- if packet.isEnum %}
		enum {{ packet.name | typeName }}
		{
			{%- for element in packet.iter() %}
			{%- if element.description %}
			/** {{ element.description | xpcc.wordwrap(63) | xpcc.indent(3, " * ") }} */
			{%- endif %}
			{{ element.name | enumElement }} = {{ element.value }},
			{%- endfor %}
		} __attribute__((packed));
		
		inline const char* 
		enumToString({{ packet.name | typeName }} e)
		{
			switch (e)
			{
			{%- for element in packet.iter() %}
			{%- if element.string %}
				case {{ element.name | enumElement }}: return "{{ element.string }}";
			{%- else %}
				case {{ element.name | enumElement }}: return "{{ element.name | enumElement }}";
			{%- endif %}
			{%- endfor %}
				default: return "__UNKNOWN_ENUM__";
			}
		}
	{% elif packet.isEnumClass %}
		enum class
		{%- if packet.underlyingType == None %}
		{{ packet.name | typeName }}
		{%- else %}
		{{ packet.name | typeName }} : {{ packet.underlyingType }}
		{%- endif %}
		{
			{%- for element in packet.iter() %}
			{%- if element.description %}
			/** {{ element.description | xpcc.wordwrap(63) | xpcc.indent(3, " * ") }} */
			{%- endif %}
			{{ element.name }} = {{ element.value }},
			{%- endfor %}
		};

		inline const char*
		enumToString({{ packet.name | typeName }} e)
		{
			switch (e)
			{
			{%- for element in packet.iter() %}
			{%- if element.string %}
				case {{ packet.name | typeName }}::{{ element.name }}: return "{{ packet.name | typeName }}::{{ element.string }}";
			{%- else %}
				case {{ packet.name | typeName }}::{{ element.name }}: return "{{ packet.name | typeName }}::{{ element.name }}";
			{%- endif %}
			{%- endfor %}
				default: return "{{ packet.name | typeName }}::Unknown";
			}
		}
	{% elif packet.isStruct %}
		struct {{ packet.name | typeName }}
		{
			{{ packet.flattened() | generateConstructor }};
			
			{% if packet.flattened().size > 0 -%}
			{{ packet.flattened() | generateConstructor(default=False) }};
			{%- endif %}
			{% for element in packet.flattened().iter() %}
			{%- if element.description %}
			/** {{ element.description | xpcc.wordwrap(63) | xpcc.indent(3, " * ") }} */
			{%- endif %}
			{{ element | subtype }};
			{%- endfor %}
		} __attribute__((packed));
	{% elif packet.isTypedef %}
		typedef {{ packet.subtype.name | typeName }} {{ packet.name | typeName }};
	{% endif %}
{%- endfor -%}
	} // namespace packet
} // namespace robot

#endif	// ROBOT__PACKETS_HPP

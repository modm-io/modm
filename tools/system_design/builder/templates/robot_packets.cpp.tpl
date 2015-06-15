// ----------------------------------------------------------------------------
/*
 * WARNING: This file is generated automatically, do not edit!
 * Please modify the corresponding XML file instead.
 */
// ----------------------------------------------------------------------------

#include {{ includeDirective }}

{% for packet in packets -%}
{%- if packet.flattened().isStruct and packet.flattened().size > 0-%}
{{ namespace }}::packet::{{ packet.flattened().name | typeName }}::{{ packet.flattened() | generateConstructor }} :
	{{ packet.flattened() | generateInitializationList }}
{
}

{{ namespace }}::packet::{{ packet.flattened().name | typeName }}::{{ packet.flattened() | generateConstructor(default=False) }} :
	{{ packet.flattened() | generateInitializationList(default=False) }}
{
}
{% endif %}
{%- endfor %}

// ----------------------------------------------------------------------------
// IOStream Helpers
{%- for packet in packets %}
	{%- if packet.isBuiltIn %}{% continue %}{% endif %}

	{%- if packet.isEnum %}
xpcc::IOStream&
{{ namespace }}::packet::operator << (xpcc::IOStream& s, const {{ packet.name | typeName }} e)
{
	s << "{{ (packet.name | typeName) ~ "::" }}" << enumToString(e);
	return s;
}
	{% elif packet.flattened().isStruct %}
xpcc::IOStream&
{{ namespace }}::packet::operator << (xpcc::IOStream& s, const {{ packet.flattened().name | typeName }} e)
{
	s << "{{ packet.flattened().name | typeName }}(";
		{%- for element in packet.flattened().iter() %}
	s << " {{ element.name | variableName }}=" << e.{{ element.name | variableName }};
		{%- endfor %}
	s << " )";
	return s;
}
	{%- endif %}
{%- endfor -%}

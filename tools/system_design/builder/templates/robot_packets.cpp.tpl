// ----------------------------------------------------------------------------
/*
 * WARNING: This file is generated automatically, do not edit!
 * Please modify the corresponding XML file instead.
 */
// ----------------------------------------------------------------------------

#include {{ includeDirective }}

{% for packet in packets -%}
{%- if packet.flattened().isStruct -%}
robot::packet::{{ packet.flattened().name | typeName }}::{{ packet.flattened() | generateConstructor }} :
	{{ packet.flattened() | generateInitializationList }}
{
}

robot::packet::{{ packet.flattened().name | typeName }}::{{ packet.flattened() | generateConstructor(default=False) }} :
	{{ packet.flattened() | generateInitializationList(default=False) }}
{
}
{% endif %}
{%- endfor -%}

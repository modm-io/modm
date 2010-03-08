// ----------------------------------------------------------------------------
/*
 * WARNING: This file is generated automatically, do not edit!
 * Please modify the corresponding XML file instead.
 */
// ----------------------------------------------------------------------------

#include <robot/packets.hpp>

{% for packet in packets -%}
{%- if packet.isStruct -%}
robot::packet::{{ packet.name | typeName }}::{{ packet | generateConstructor }} :
	{{ packet | generateInitializationList }}
{
}

robot::packet::{{ packet.name | typeName }}::{{ packet | generateConstructor(default=False) }} :
	{{ packet | generateInitializationList(default=False) }}
{
}
{% endif %}
{%- endfor -%}

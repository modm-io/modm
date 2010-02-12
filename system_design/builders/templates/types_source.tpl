// ----------------------------------------------------------------------------
/*
 * WARNING: This file is generated automatically, do not edit!
 * Please modify the corresponding XML file instead.
 *
 * Generated {{ time }}
 */
// ----------------------------------------------------------------------------

#include "robot_packets.hpp"

{% for type in types -%}
{% if type.is_struct %}
robot::type::{{ type.name | cpp.type }}() :
	{% for item in type.iter() -%}
	{{ item.name | cpp.variable }}({{ item.name | cpp.variable }}),
	{%- endfor %}
{
}
{% endif %}
{%- endfor -%}

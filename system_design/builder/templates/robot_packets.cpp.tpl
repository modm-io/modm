// ----------------------------------------------------------------------------
/*
 * WARNING: This file is generated automatically, do not edit!
 * Please modify the corresponding XML file instead.
 */
// ----------------------------------------------------------------------------

#include <robot/packets.hpp>

{% for type in types -%}
{%- if type.is_struct -%}
robot::packet::{{ type.name | cpp.type }}::{{ type | cpp.constructor(True) }} :
	{{ type | cpp.initialization_list(True) }}
{
}

robot::packet::{{ type.name | cpp.type }}::{{ type | cpp.constructor(False) }} :
	{{ type | cpp.initialization_list(False) }}
{
}
{% endif %}
{%- endfor -%}

#
# WARNING: This file is generated automatically, do not edit!
# Please modify the corresponding XML file instead.
#
# ----------------------------------------------------------------------------

{% for primitive in primitives -%}
#Packets${{ primitive.name }}
{% endfor %}
	
{% for packet in packets -%}
{%- if packet.isStruct or packet.isEnum or packet.isTypedef %}
Packets${{ packet.flattened().name | typeName }}
{%- endif %}
{%- endfor %}

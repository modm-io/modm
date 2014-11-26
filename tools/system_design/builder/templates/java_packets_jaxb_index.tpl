#
# WARNING: This file is generated automatically, do not edit!
# Please modify the corresponding XML file instead.
#
# ----------------------------------------------------------------------------

{% for primitive in primitives %}
#Packets${{ primitive.name }}
{% endfor %}
	
{%- for packet in packets %}
Packets${{ packet.flattened().name | typeName }}
{%- endfor %}

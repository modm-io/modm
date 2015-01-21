#
# WARNING: This file is generated automatically from java_packets_jaxb_index.tpl
# Do not edit! Please modify the corresponding XML file instead.
#
# ----------------------------------------------------------------------------

Packets$Void

{% for primitive in primitives -%}
Packets${{ primitive.name }}
{% endfor -%}

{% for packet in packets -%}
{%- if packet.isStruct or packet.isEnum or packet.isTypedef %}
Packets${{ packet.flattened().name | typeName }}
{%- endif %}
{%- endfor %}

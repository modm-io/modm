%# Copyright (c) 2014, Georgi Grinshpun
%# Copyright (c) 2015, Sascha Schade
%#
%# This file is part of the modm project.
%#
%# This Source Code Form is subject to the terms of the Mozilla Public
%# License, v. 2.0. If a copy of the MPL was not distributed with this
%# file, You can obtain one at http://mozilla.org/MPL/2.0/.
%# ----------------------------------------------------------------------------
#
# WARNING: This file is generated automatically from java_packets_jaxb_index.tpl
# Do not edit! Please modify the corresponding XML file instead.
#
# -----------------------------------------------------------------------------

Packets$Void

{% for primitive in primitives -%}
Packets${{ primitive.name }}
{% endfor -%}

{% for packet in packets -%}
{%- if packet.isStruct or packet.isEnum or packet.isTypedef %}
Packets${{ packet.flattened().name | typeName }}
{%- endif %}
{%- endfor %}

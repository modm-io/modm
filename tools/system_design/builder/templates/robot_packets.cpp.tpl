%# Copyright (c) 2010-2012, Fabian Greif
%# Copyright (c) 2015, Niklas Hauser
%# Copyright (c) 2015, Sascha Schade
%# Copyright (c) 2016, Kevin Laeufer
%#
%# This file is part of the modm project.
%#
%# This Source Code Form is subject to the terms of the Mozilla Public
%# License, v. 2.0. If a copy of the MPL was not distributed with this
%# file, You can obtain one at http://mozilla.org/MPL/2.0/.
%# ----------------------------------------------------------------------------
/*
 * WARNING: This file is generated automatically, do not edit!
 * Please modify the corresponding XML file instead.
 */
// ----------------------------------------------------------------------------

#include {{ includeDirective }}

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

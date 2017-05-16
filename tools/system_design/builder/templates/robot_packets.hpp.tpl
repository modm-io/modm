%# Copyright (c) 2010-2011, Martin Rosekeit
%# Copyright (c) 2010-2012, Fabian Greif
%# Copyright (c) 2013, 2015, Sascha Schade
%# Copyright (c) 2015, Niklas Hauser
%# Copyright (c) 2016, Kevin Läufer
%#
%# This file is part of the modm project.
%#
%# This Source Code Form is subject to the terms of the Mozilla Public
%# License, v. 2.0. If a copy of the MPL was not distributed with this
%# file, You can obtain one at http://mozilla.org/MPL/2.0/.
%# ----------------------------------------------------------------------------
/*
 * WARNING: This file is generated automatically from robot_packets.hpp.tpl.
 * Do not edit! Please modify the corresponding XML file instead.
 */
// ----------------------------------------------------------------------------

#ifndef	{{ namespace | upper }}_PACKETS_HPP
#define	{{ namespace | upper }}_PACKETS_HPP

#include <stdint.h>
#include <cstdlib>
#include <modm/io/iostream.hpp>
#include <modm/container/smart_pointer.hpp>

namespace {{ namespace }}
{
	namespace packet
	{
{%- for packet in packets %}
{%- if packet.isBuiltIn %}{% continue %}{% endif %}
		{%- if packet.description %}
		/** {{ packet.description | modm.wordwrap(67) | modm.indent(2, " * ") }} */
		{%- endif %}
	{%- if packet.isEnum %}
		enum {%- if packet.isStronglyTyped %} class{%- endif %}
		{{ packet.name | typeName }}{%- if packet.underlyingType != None %} : {{ packet.underlyingType }}{%- endif %}
		{
			{%- for element in packet.iter() %}
				{%- if element.description %}
			/** {{ element.description | modm.wordwrap(63) | modm.indent(3, " * ") }} */
				{%- endif %}
				{%- if packet.isStronglyTyped %}
			{{ element.name | enumElementStrong }} = {{ element.value }},
				{%- else %}
			{{ element.name | enumElement }} = {{ element.value }},
				{%- endif %}
			{%- endfor %}
		} {%- if packet.underlyingType == None %} __attribute__((packed)){%- endif %};
		{% if packet.isStronglyTyped %}
		constexpr {{ packet.underlyingType }}
		value({{ packet.name | typeName }} e) {
			return {{ packet.underlyingType }}(e);
		}
		{%- endif %}

		static constexpr int {{ packet.name | typeName }}NumberOfElements = {{ packet.numberOfElements }};

		inline const char*
		enumToString({{ packet.name | typeName }} e)
		{
			switch (e)
			{
		{%- for element in packet.iter() %}
			{%- if packet.isStronglyTyped %}
				{%- set enumName = element.name | enumElementStrong %}
				{%- set prefix = (packet.name | typeName) ~ "::" %}
			{%- else %}
				{%- set enumName = element.name | enumElement %}
				{%- set prefix = "" %}
			{%- endif %}
			{%- if element.string %}
				case {{ prefix ~ enumName }}: return "{{ element.string }}";
			{%- else %}
				case {{ prefix ~ enumName }}: return "{{ enumName }}";
			{%- endif %}
		{%- endfor %}
		{%- if packet.isStronglyTyped %}
				default: return "Unknown";
		{%- else %}
				default: return "__UNKNOWN_ENUM__";
		{%- endif %}
			}
		}

		modm::IOStream&
		operator << (modm::IOStream& s, const {{ packet.name | typeName }} e);

	{% elif packet.isStruct %}
		struct {{ packet.name | typeName }}
		{
			constexpr {{ packet.flattened() | generateConstructor }}:
				{{ packet.flattened() | generateInitializationList }} {}

			{% if packet.flattened().size > 0 -%}
			constexpr {{ packet.flattened() | generateConstructor(default=False) }} :
				{{ packet.flattened() | generateInitializationList(default=False) }} {}
			{%- endif %}
			{% for element in packet.flattened().iter() %}
			{%- if element.description %}
			/** {{ element.description | modm.wordwrap(63) | modm.indent(3, " * ") }} */
			{%- endif %}
			{{ element | subtype }};
			{%- endfor %}
		} __attribute__((packed));

		modm::IOStream&
		operator << (modm::IOStream& s, const {{ packet.name | typeName }} e);

	{% elif packet.isTypedef %}
		{%- if packet.subtype.type.isBuiltIn %}
		typedef {{ packet.subtype.name }} {{ packet.name | typeName }};
		{%- else %}
		typedef {{ packet.subtype.name | typeName }} {{ packet.name | typeName }};
		{%- endif %}
	{% endif %}
{%- endfor -%}
	} // namespace packet
} // namespace {{ namespace }}

#endif	// {{ namespace | upper }}_PACKETS_HPP

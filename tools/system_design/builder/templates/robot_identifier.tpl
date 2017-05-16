%# Copyright (c) 2010-2011, Fabian Greif
%# Copyright (c) 2011, 2014, Martin Rosekeit
%# Copyright (c) 2014, 2016, Sascha Schade
%# Copyright (c) 2015, Georgi Grinshpun
%# Copyright (c) 2015, Niklas Hauser
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

#ifndef	{{ namespace | upper }}_IDENTIFIER_HPP
#define	{{ namespace | upper }}_IDENTIFIER_HPP

#include <stdint.h>

namespace {{ namespace }}
{
	namespace domain
	{
		enum Identifier
		{
		{%- for item in domains -%}
		{%- if item.id != None %}
			{{ item.name | enumElement }} = {{ item.id | enumValue }},
		{%- endif -%}
		{%- endfor %}
		};

		inline const char*
		enumToString(Identifier e)
		{
			switch (e)
			{
			{%- for item in domains %}
			{%- if item.id != None %}
				case {{ item.name | enumElement }}: return "{{ item.name | enumElement }}";
			{%- endif -%}
			{%- endfor %}
				default: return "__UNKNOWN_DOMAIN__";
			}
		}
	}

	namespace container
	{
		enum class Identifier : uint8_t
		{
		{%- for item in containers %}
			{{ item.name | enumElementStrong }} = {{ item.id | enumValue }},
		{%- endfor %}
		};
	}

	namespace component
	{
		enum Identifier
		{
		{%- for item in components %}
			{{ item.name | enumElement }} = {{ item.id | enumValue }},
		{%- endfor %}
		};

		inline const char*
		enumToString(Identifier e)
		{
			switch (e)
			{
			{%- for item in components %}
				case {{ item.name | enumElement }}: return "{{ item.name | enumElement }}";
			{%- endfor %}
				default: return "__UNKNOWN_COMPONENT__";
			}
		}
	}

	namespace action
	{
		enum Identifier
		{
		{%- for item in actions %}
			{{ item.name | enumElement }} = {{ item.id | enumValue }},
		{%- endfor %}
		};

		inline const char*
		enumToString(Identifier e)
		{
			switch (e)
			{
			{%- for item in actions %}
				case {{ item.name | enumElement }}: return "{{ item.name | enumElement }}";
			{%- endfor %}
				default: return "__UNKNOWN_ACTION__";
			}
		}
	}

	namespace event
	{
		enum Identifier
		{
		{%- for item in events %}
			{{ item.name | enumElement }} = {{ item.id | enumValue }},
		{%- endfor %}
		};

		inline const char*
		enumToString(Identifier e)
		{
			switch (e)
			{
			{%- for item in events %}
				case {{ item.name | enumElement }}: return "{{ item.name | enumElement }}";
			{%- endfor %}
				default: return "__UNKNOWN_EVENT__";
			}
		}
	}
}	// namespace {{ namespace }}

#endif	// {{ namespace | upper }}_IDENTIFIER_HPP

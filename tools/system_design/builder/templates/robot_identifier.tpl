// ----------------------------------------------------------------------------
/*
 * WARNING: This file is generated automatically, do not edit!
 * Please modify the corresponding XML file instead.
 */
// ----------------------------------------------------------------------------

#ifndef	ROBOT__IDENTIFIER_HPP
#define	ROBOT__IDENTIFIER_HPP

namespace robot
{
	namespace domain
	{
		enum Identifier
		{
		{%- for item in domains -%}
		{%- if item.id != None %}
			{{ item.name | upper | replace(' ', '_') }} = {{ "0x%02x"|format(item.id) }},
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
				case {{ item.name | upper | replace(' ', '_') }}: return "{{ item.name | upper | replace(' ', '_') }}";
			{%- endif -%}
			{%- endfor %}
				default: return "__UNKNOWN_DOMAIN__";
			}
		}
	}
	
	namespace component
	{
		enum Identifier
		{
		{%- for item in components %}
			{{ item.name | upper | replace(' ', '_') }} = {{ "0x%02x"|format(item.id) }},
		{%- endfor %}
		};
				
		inline const char* 
		enumToString(Identifier e)
		{
			switch (e)
			{
			{%- for item in components %}
				case {{ item.name | upper | replace(' ', '_') }}: return "{{ item.name | upper | replace(' ', '_') }}";
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
			{{ item.name | upper | replace(' ', '_') }} = {{ "0x%02x"|format(item.id) }},
		{%- endfor %}
		};
				
		inline const char* 
		enumToString(Identifier e)
		{
			switch (e)
			{
			{%- for item in actions %}
				case {{ item.name | upper | replace(' ', '_') }}: return "{{ item.name | upper | replace(' ', '_') }}";
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
			{{ item.name | upper | replace(' ', '_') }} = {{ "0x%02x"|format(item.id) }},
		{%- endfor %}
		};
		
		inline const char* 
		enumToString(Identifier e)
		{
			switch (e)
			{
			{%- for item in events %}
				case {{ item.name | upper | replace(' ', '_') }}: return "{{ item.name | upper | replace(' ', '_') }}";
			{%- endfor %}
				default: return "__UNKNOWN_EVENT__";
			}
		}
	}
}

#endif	// ROBOT__IDENTIFIER_HPP

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
	namespace component
	{
		enum Identifier
		{
		{%- for item in components %}
			{{ item.name | upper | replace(' ', '_') }} = {{ item.id }},
		{%- endfor %}
		};
	}
	
	namespace action
	{
		enum Identifier
		{
		{%- for item in actions %}
			{{ item.name | upper | replace(' ', '_') }} = {{ item.id }},
		{%- endfor %}
		};
	}
		
	namespace event
	{
		enum Identifier
		{
		{%- for item in events %}
			{{ item.name | upper | replace(' ', '_') }} = {{ item.id }},
		{%- endfor %}
		};
	}
}

#endif	// ROBOT__IDENTIFIER_HPP

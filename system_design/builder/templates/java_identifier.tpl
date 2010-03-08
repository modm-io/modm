/*
 * WARNING: This file is generated automatically, do not edit!
 * Please modify the corresponding XML file instead.
 */
// ----------------------------------------------------------------------------

package rca10.robot;

public class Identifier
{
	public static enum Component
	{
	{%- for element in components.iter() %}
		{{ element.name | enumElement }}({{ element.id }}){% if loop.last %};{% else %},{% endif %}
	{%- endfor %}

		public final int id;
		private Component(int id) {
			this.id = id;
		}
	}
	
	public static enum Action
	{
	{%- for element in actions.iter() %}
		{{ element.name | enumElement }}({{ element.id }}){% if loop.last %};{% else %},{% endif %}
	{%- endfor %}

		public final int id;
		private Action(int id) {
			this.id = id;
		}
	}
	
	public static enum Event
	{
	{%- for element in events.iter() %}
		{{ element.name | enumElement }}({{ element.id }}){% if loop.last %};{% else %},{% endif %}
	{%- endfor %}

		public final int id;
		private Event(int id) {
			this.id = id;
		}
	}
}

/*
 * WARNING: This file is generated automatically, do not edit!
 * Please modify the corresponding XML file instead.
 */
// ----------------------------------------------------------------------------

package {{ package }};

import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;

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
		{% if element.type == None %}
		{{ element.name | enumElement }}({{ element.id }}, null){% if loop.last %};{% else %},{% endif %}
		{% else %}
		{{ element.name | enumElement }}({{ element.id }}, Packets.{{ element.type | typeObjectName }}.class){% if loop.last %};{% else %},{% endif %}
		{% endif %}
	{%- endfor %}

		public final int id;
		public final Class<?> eventType;
		private Event(int id, Class<?> eventType) {
			this.id = id;
			this.eventType = eventType;
		}
		
		@SuppressWarnings("unchecked")
		public <T extends Packets.Packet> T getPayload(byte[] payload){
			if (eventType == null)
				return null;
			try {
				Method fromBuffer = eventType.getMethod("fromBuffer", byte[].class);
				Object p = fromBuffer.invoke(null, payload);
				return (T)p;
			}
			catch (SecurityException e) {
				e.printStackTrace();
			}
			catch (NoSuchMethodException e) {
				e.printStackTrace();
			}
			catch (IllegalArgumentException e) {
				e.printStackTrace();
			}
			catch (IllegalAccessException e) {
				e.printStackTrace();
			}
			catch (InvocationTargetException e) {
				e.printStackTrace();
			}
			return null;
		}
	}
}
